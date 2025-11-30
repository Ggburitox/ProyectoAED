#include "crow_all.h"
#include "json.hpp"
#include <vector>
#include <cmath>
#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;
using json = nlohmann::json;

// --- MIDDLEWARE PARA CORS (Permite que React se comunique con C++) ---
struct CORSHandler {
    struct context {};
    void before_handle(crow::request& req, crow::response& res, context& ctx) {}
    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    }
};

class LinearHashing {
private:
    int i = 0;              // Nivel actual
    int N = 1;              // Número total de buckets
    int p = 0;              // Puntero Next to Split
    int capacity = 4;       // Capacidad del bucket
    int initial_n = 1;      // N0
    
    // AJUSTE CLAVE: Subimos el umbral a 0.85 (85%)
    // Esto evita splits prematuros y permite que se formen overflows como en tu video.
    // El PDF menciona que LH puede operar eficientemente con factores > 90%.
    float max_load = 0.90;  
    float min_load = 0.50;  // Umbral para Contracción (50%) segun PDF [cite: 386]
    
    vector<vector<int>> buckets;
    map<int, vector<int>> overflow; 
    vector<string> logs;
    int total_elements = 0;

    void log(string msg) { logs.push_back(msg); }

    // Función Hash: h_i(k) = k mod (N0 * 2^i) [cite: 149]
    int hash(int key, int level) {
        int m = initial_n * (int)pow(2, level);
        return key % m;
    }

    // Verificar duplicados en buckets y overflows
    bool exists(int key) {
        for(auto& b : buckets) for(int val : b) if(val == key) return true;
        for(auto const& [idx, vec] : overflow) for(int val : vec) if(val == key) return true;
        return false;
    }

    // Dirección Lógica [cite: 193]
    int get_address(int key) {
        int addr = hash(key, i);
        if (addr < p) {
            addr = hash(key, i + 1);
        }
        return addr;
    }

    // Helper para insertar físicamente (maneja la creación de overflow)
    void insert_physically(int idx, int val) {
        if (buckets[idx].size() < capacity) {
            buckets[idx].push_back(val);
        } else {
            overflow[idx].push_back(val);
        }
    }

    // --- SPLIT: SIEMPRE DIVIDE EL BUCKET P [cite: 159] ---
    void perform_split() {
        int split_target = p; // El bucket apuntado por p se divide [cite: 159]
        
        log("⚡ EJECUTANDO SPLIT en Bucket " + to_string(split_target) + " (puntero p)");

        // A. Crear nuevo bucket vacío al final de la tabla
        buckets.push_back(vector<int>()); 
        int new_bucket_idx = N;
        N++;

        // B. Recolectar datos SOLAMENTE del bucket P y su Overflow
        // El PDF indica redistribuir elementos del bucket p[cite: 160].
        // Si hay overflow en otros buckets, NO SE TOCAN AÚN.
        vector<int> items_to_redistribute = buckets[split_target];
        
        if (overflow.count(split_target)) {
            items_to_redistribute.insert(items_to_redistribute.end(), overflow[split_target].begin(), overflow[split_target].end());
            overflow.erase(split_target); // Eliminamos el overflow de ESTE bucket porque lo vamos a reordenar
            log("   -> Absorbiendo overflow del Bucket " + to_string(split_target) + " para redistribuir.");
        }

        // C. Vaciar el bucket P físico
        buckets[split_target].clear();

        // D. Redistribuir los items entre P y el Nuevo Bucket
        // Usamos h_{i+1} [cite: 160]
        for (int val : items_to_redistribute) {
            int h_new = hash(val, i + 1);
            
            // h_new solo puede dar dos resultados: 'split_target' o 'new_bucket_idx'
            int dest = (h_new == split_target) ? split_target : new_bucket_idx;
            
            insert_physically(dest, val);
        }

        // E. Avanzar el puntero P [cite: 161]
        int old_p = p;
        p++;
        
        // F. Verificar si terminamos la ronda
        int cycle_limit = initial_n * (int)pow(2, i);
        if (p == cycle_limit) {
            log("🔄 Fin de Ronda. Todos los buckets base divididos.");
            log("➡ Puntero p (" + to_string(old_p) + ") -> 0. Nivel i (" + to_string(i) + ") -> " + to_string(i+1));
            p = 0;
            i++;
        } else {
            log("➡ Puntero p avanza: " + to_string(old_p) + " -> " + to_string(p));
        }
    }

    // --- CONTRACCIÓN (Grouping) [cite: 230] ---
    void perform_contraction() {
        if (N <= initial_n) return; // No contraer si estamos en el mínimo

        int last_bucket_idx = N - 1;
        int old_p = p;
        
        // Retroceder Puntero (Inverso a avanzar p)
        if (p == 0) {
             if (i > 0) {
                i--;
                p = (initial_n * (int)pow(2, i)) - 1;
                log("⬅ Contracción: Nivel baja a " + to_string(i) + ". Puntero p regresa a " + to_string(p));
             }
        } else {
            p--;
            log("⬅ Puntero p retrocede: " + to_string(old_p) + " -> " + to_string(p));
        }

        int parent_bucket = p; 
        log("🔙 CONTRACCIÓN: Fusionando Bucket " + to_string(last_bucket_idx) + " en " + to_string(parent_bucket));

        // Recoger items del último bucket + su overflow
        vector<int> items = buckets[last_bucket_idx];
        if (overflow.count(last_bucket_idx)) {
            items.insert(items.end(), overflow[last_bucket_idx].begin(), overflow[last_bucket_idx].end());
            overflow.erase(last_bucket_idx);
        }

        // Eliminar el último bucket físicamente [cite: 387]
        buckets.pop_back();
        N--;

        // Reinsertar en el padre [cite: 388]
        for(int val : items) {
            insert_physically(parent_bucket, val);
        }
    }

public:
    LinearHashing() { buckets.resize(1); }

    // --- LÓGICA CORE: INSERTAR Y LUEGO DECIDIR SI DIVIDIR ---
    json insert(int key) {
        logs.clear();
        
        // 1. Verificar duplicados
        if (exists(key)) {
            log("Clave " + to_string(key) + " ya existe. Omitiendo.");
            return get_state();
        }

        // 2. Calcular dónde va (Dirección Lógica)
        int addr = get_address(key);
        
        // Log para mostrar qué fórmula se usó
        int m = initial_n * (int)pow(2, i);
        if (addr < p) m *= 2; 
        log("Insertar " + to_string(key) + ": " + to_string(key) + " mod " + to_string(m) + " = " + to_string(addr));

        bool caused_overflow = false;
        
        // 3. INSERCIÓN FÍSICA
        // Aquí es donde puede ocurrir el overflow.
        if (buckets[addr].size() < capacity) {
            buckets[addr].push_back(key);
            log("Insertado en Bucket " + to_string(addr));
        } else {
            // ¡OVERFLOW! Se guarda temporalmente y se queda ahí[cite: 158].
            overflow[addr].push_back(key);
            caused_overflow = true;
            log("⚠️ Bucket " + to_string(addr) + " LLENO. Se crea Overflow temporal.");
        }
        total_elements++;

        // 4. DECISIÓN DE SPLIT
        // Dividimos si hay overflow O carga excesiva (> 85%)
        float load = (float)total_elements / (capacity * N);
        
        if (caused_overflow || load > max_load) {
            log("🚨 Condición de Split cumplida (Overflow o Carga > " + to_string((int)(max_load*100)) + "%)");
            perform_split(); // Esto dividirá buckets[p]
        }
        
        return get_state();
    }

    // --- LÓGICA DE ELIMINACIÓN [cite: 229] ---
    json remove(int key) {
        logs.clear();
        int addr = get_address(key);
        bool found = false;
        
        // Buscar y borrar en bucket principal
        auto& b = buckets[addr];
        auto it = find(b.begin(), b.end(), key);
        if (it != b.end()) {
            b.erase(it);
            found = true;
        } 
        // Buscar y borrar en overflow
        else if (overflow.count(addr)) {
            auto& vec = overflow[addr];
            auto it2 = find(vec.begin(), vec.end(), key);
            if (it2 != vec.end()) {
                vec.erase(it2);
                if(vec.empty()) overflow.erase(addr);
                found = true;
            }
        }

        if (found) {
            total_elements--;
            log("Eliminado " + to_string(key));
            
            // Verificar Contracción (Load Factor < 50%) [cite: 386]
            float load = (float)total_elements / (capacity * N);
            if (load < min_load && N > initial_n) {
                log("Carga baja (" + to_string(load*100).substr(0,4) + "%). Iniciando Contracción.");
                perform_contraction();
            }
        } else {
            log("Clave " + to_string(key) + " no encontrada.");
        }
        return get_state();
    }

    json get_state() {
        json j;
        j["level"] = i;
        j["N"] = N;
        j["p"] = p;
        j["buckets"] = buckets;
        j["overflow"] = overflow;
        j["logs"] = logs;
        return j;
    }
    
    void reset() {
        i = 0; N = 1; p = 0; total_elements = 0;
        buckets.clear(); buckets.resize(1);
        overflow.clear(); logs.clear();
    }
};

int main() {
    crow::App<CORSHandler> app;
    LinearHashing lh;

    CROW_ROUTE(app, "/state")([&lh](){ return lh.get_state().dump(); });

    CROW_ROUTE(app, "/insert").methods("POST"_method)([&lh](const crow::request& req){
        auto x = json::parse(req.body);
        return lh.insert(x["key"]).dump();
    });

    CROW_ROUTE(app, "/delete").methods("POST"_method)([&lh](const crow::request& req){
        auto x = json::parse(req.body);
        return lh.remove(x["key"]).dump();
    });
    
    CROW_ROUTE(app, "/reset").methods("POST"_method)([&lh](){
        lh.reset();
        return lh.get_state().dump();
    });

    app.port(8080).multithreaded().run();
}