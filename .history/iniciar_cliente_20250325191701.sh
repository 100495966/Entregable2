#!/bin/bash

# Script para iniciar 3 clientes diferentes

# Definir directorios y ejecutables basados en el Makefile
BIN_DIR="bin"
CLIENT_EXEC1="app-cliente-1"
CLIENT_PATH1="$BIN_DIR/$CLIENT_EXEC1"

# Exportar las variables de entorno
export IP_TUPLAS="0.0.0.0"  # La IP del servidor
export PORT_TUPLAS="4500"     # El puerto donde está escuchando el servidor

echo "Iniciando cliente"
env IP_TUPLAS=0.0.0.0 PORT_TUPLAS=4500 $CLIENT_PATH1 &
