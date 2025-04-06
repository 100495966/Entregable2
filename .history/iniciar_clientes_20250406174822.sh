#!/bin/bash

# Script para iniciar 3 clientes diferentes

# Definir directorios y ejecutables basados en el Makefile
BIN_DIR="bin"
CLIENT_EXEC1="app-cliente-1"
CLIENT_EXEC2="app-cliente-2"
CLIENT_EXEC3="app-cliente-3"
CLIENT_EXEC4="app-cliente-4"
CLIENT_PATH1="$BIN_DIR/$CLIENT_EXEC1"
CLIENT_PATH2="$BIN_DIR/$CLIENT_EXEC2"
CLIENT_PATH3="$BIN_DIR/$CLIENT_EXEC3"
CLIENT_PATH4="$BIN_DIR/$CLIENT_EXEC4"

# Exportar las variables de entorno
export IP_TUPLAS="localhost"  # La IP del servidor
export PORT_TUPLAS="4500"     # El puerto donde está escuchando el servidor

echo "Iniciando cliente 1"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 $CLIENT_PATH1 &

echo "Iniciando cliente 2"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 $CLIENT_PATH2 &

echo "Iniciando cliente 3"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 $CLIENT_PATH3 &

echo "Iniciando cliente 4"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 $CLIENT_PATH4 &