#!/bin/bash

# Script para iniciar 4 clientes diferentes

# Definir ejecutables basados en el Makefile
CLIENT_EXEC1="app-cliente-1"
CLIENT_EXEC2="app-cliente-2"
CLIENT_EXEC3="app-cliente-3"
CLIENT_EXEC4="app-cliente-4"

# Exportar las variables de entorno
export IP_TUPLAS="localhost"  # La IP del servidor
export PORT_TUPLAS="4500"     # El puerto donde está escuchando el servidor

echo "Iniciando cliente 1"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 ./$CLIENT_EXEC1 &

echo "Iniciando cliente 2"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 ./$CLIENT_EXEC2 &

echo "Iniciando cliente 3"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 ./$CLIENT_EXEC3 &

echo "Iniciando cliente 4"
env IP_TUPLAS=localhost PORT_TUPLAS=4500 ./$CLIENT_EXEC4 &