#!/bin/bash

# Script para iniciar 3 clientes diferentes

# Definir directorios y ejecutables basados en el Makefile
BIN_DIR="bin"
CLIENT_EXEC1="app-cliente-1"
CLIENT_EXEC2="app-cliente-2"
CLIENT_EXEC3="app-cliente-3"
CLIENT_PATH1="$BIN_DIR/$CLIENT_EXEC1"
CLIENT_PATH2="$BIN_DIR/$CLIENT_EXEC2"
CLIENT_PATH3="$BIN_DIR/$CLIENT_EXEC3"

# Exportar las variables de entorno
export IP_TUPLAS="localhost"  # La IP del servidor
export PORT_TUPLAS="4500"     # El puerto donde está escuchando el servidor

# Iniciar los 3 clientes diferentes
echo "Iniciando 3 clientes diferentes..."
CLIENT_PIDS=()

echo "Iniciando cliente 1"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 $CLIENT_PATH1 &
CLIENT_PIDS+=($!)

echo "Iniciando cliente 2"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 $CLIENT_PATH2 &
CLIENT_PIDS+=($!)

echo "Iniciando cliente 3"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 $CLIENT_PATH3 &
CLIENT_PIDS+=($!)