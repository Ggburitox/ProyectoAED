# Usamos Debian Slim (versión muy ligera de Linux)
FROM debian:bullseye-slim

# Evitar preguntas de instalación
ENV DEBIAN_FRONTEND=noninteractive

# 1. Instalar SOLO el compilador y wget (para descargar las libs)
# Quitamos CMake y Boost completo para ahorrar cientos de MBs
RUN apt-get update && apt-get install -y \
    g++ \
    wget \
    libboost-system-dev \
    libboost-thread-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 2. Descargar las librerías "header-only" (ligeras)
RUN mkdir src
RUN wget -O src/crow_all.h https://github.com/CrowCpp/Crow/releases/download/v1.0+5/crow_all.h
RUN wget -O src/json.hpp https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp

# 3. Copiar tu código
COPY . .

# 4. Compilar MANUALMENTE (Esto es lo que lo hace rápido)
# Sin CMake, directo al grano. Tarda segundos.
RUN g++ src/main.cpp -o server -lpthread -I src -O3

CMD ["./server"]