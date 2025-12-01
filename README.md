# 🐳 Proyecto AED – Linear Hashing (Litwin)

Este repositorio contiene la implementación completa del algoritmo **Linear Hashing** propuesto por Litwin, incluyendo las operaciones de:

* Inserción
* Eliminación
* Split (expansión)
* Grouping (contracción)
* Manejo de buckets y overflow
* Validación de integridad y simulación del proceso

El proyecto está configurado para ejecutarse dentro de un entorno **Docker**, y se recomienda utilizar **CLion** para su desarrollo y ejecución.

---

## 📦 Requisitos

* **Docker Desktop**
* **CLion (JetBrains)**
* Plugin de **Docker** habilitado en CLion
* **Git**

---

## 🚀 Ejecución del Proyecto

### 1️⃣ Clonar el repositorio

git clone https://github.com/Ggburitox/ProyectoAED
cd ProyectoAED

### 2️⃣ Abrir el proyecto en CLion

En CLion:

Ejemplo:

File → Open… → seleccionar la carpeta ProyectoAED/

<img width="622" height="603" alt="image" src="https://github.com/user-attachments/assets/1265ff3e-8ab1-42d7-a064-ac5b15f4cced" />


### 3️⃣ Asegurar que Docker Desktop esté ejecutándose

Docker Desktop debe estar activo antes de compilar o abrir el proyecto en CLion.


<img width="955" height="1012" alt="image" src="https://github.com/user-attachments/assets/c49359c1-4917-4b62-85a2-3475e84df6de" />


### 5️⃣ Ejecutar `docker-compose.yml`

Desde CLion:

1. Abrir `docker-compose.yml`
2. Seleccionar **Run 'docker-compose'**
3. Esperar la construcción del contenedor

   <img width="1727" height="952" alt="image" src="https://github.com/user-attachments/assets/e718de62-d619-47a6-819b-fbaf6af50963" />

### 6️⃣ Ejecutar `docker-compose.yml`

Levanta todo el entorno del proyecto desde la carpeta raíz:

```bash
docker compose up --build
```

Una vez que los servicios estén corriendo, abre **Docker Desktop** y en el panel de *Containers* selecciona el contenedor del proyecto.
Allí verás la **URL local** generada automáticamente por el frontend o backend.

Haz clic en la URL desde el dashboard para abrir la aplicación en tu navegador.

<img width="1182" height="347" alt="image" src="https://github.com/user-attachments/assets/6f9c2be2-f5ec-422a-98a0-04ed4b8fc465" />


### 7️⃣ Probar el proyecto

Una vez que la aplicación esté ejecutándose desde la URL local proporcionada por Docker Desktop, podrás probar todas las funcionalidades del Linear Hashing, incluyendo:

* Inserción de claves
* Eliminación de claves
* Visualización del split (expansión)
* Visualización del grouping (contracción)
* Estado actual de los buckets y manejo de overflow

<img width="1919" height="970" alt="image" src="https://github.com/user-attachments/assets/36811438-8a3f-4123-8c5b-236550f915fa" />


🗂 Estructura del Proyecto
ProyectoAED/
│
├── backend/            # Implementación del Linear Hashing
├── frontend/           # Interfaz visual
│
├── docker-compose.yml  # Orquestación de todos los servicios
├── README.md



## 📜 Créditos

Proyecto desarrollado para el curso **Algoritmos y Estructuras de Datos (AED)** por:

Rivera Matta, Thiago Gabriel
Martínez Olivos, Félix Alberto

