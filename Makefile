# Variables de compilación
CC = gcc
CFLAGS = -Wall -fPIC -g
LDFLAGS = -lrt -lpthread
LIBRARY_NAME = libclaves.so

# Ejecutables finales
SERVER_EXEC = servidor-sock
CLIENT_EXEC1 = app-cliente-1
CLIENT_EXEC2 = app-cliente-2
CLIENT_EXEC3 = app-cliente-3
CLIENT_EXEC4 = app-cliente-4

# Archivos fuente
CLAVES_SRC = claves.c
SERVER_SRC = servidor-sock.c
PROXY_SRC = proxy-sock.c
CLIENT_SRC1 = app-cliente-1.c
CLIENT_SRC2 = app-cliente-2.c
CLIENT_SRC3 = app-cliente-3.c
CLIENT_SRC4 = app-cliente-4.c

# Headers
HEADER_SRC = claves.h
INCL_DEF_SRC = includes_y_defines.h
SERV_PROX_SRC = servidor_y_proxy.h

# Archivos objeto
CLAVES_OBJ = claves.o
SERVER_OBJ = servidor-sock.o
PROXY_OBJ = proxy-sock.o
CLIENT_OBJ1 = app-cliente-1.o
CLIENT_OBJ2 = app-cliente-2.o
CLIENT_OBJ3 = app-cliente-3.o
CLIENT_OBJ4 = app-cliente-4.o

# Regla principal
all: $(LIBRARY_NAME) $(SERVER_EXEC) $(CLIENT_EXEC1) $(CLIENT_EXEC2) $(CLIENT_EXEC3) $(CLIENT_EXEC4) 

# Crear la biblioteca dinámica
$(LIBRARY_NAME): $(PROXY_OBJ)
	@echo "Creando la biblioteca dinámica $(LIBRARY_NAME)"
	$(CC) -shared -o $(LIBRARY_NAME) $(PROXY_OBJ) $(LDFLAGS)

$(SERVER_EXEC): $(CLAVES_OBJ) $(SERVER_OBJ)
	@echo "Compilando el servidor $(SERVER_EXEC)"
	$(CC) -o $(SERVER_EXEC) $(CLAVES_OBJ) $(SERVER_OBJ) $(LDFLAGS)

# Compilar los clientes
$(CLIENT_EXEC1): $(CLIENT_OBJ1) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC1)"
	$(CC) -o $(CLIENT_EXEC1) $(CLIENT_OBJ1) -L. -lclaves $(LDFLAGS) -Wl,-rpath,.

$(CLIENT_EXEC2): $(CLIENT_OBJ2) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC2)"
	$(CC) -o $(CLIENT_EXEC2) $(CLIENT_OBJ2) -L. -lclaves $(LDFLAGS) -Wl,-rpath,.

$(CLIENT_EXEC3): $(CLIENT_OBJ3) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC3)"
	$(CC) -o $(CLIENT_EXEC3) $(CLIENT_OBJ3) -L. -lclaves $(LDFLAGS) -Wl,-rpath,.

$(CLIENT_EXEC4): $(CLIENT_OBJ4) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC4)"
	$(CC) -o $(CLIENT_EXEC4) $(CLIENT_OBJ4) -L. -lclaves $(LDFLAGS) -Wl,-rpath,.

# Compilación de los archivos .c a .o
%.o: %.c $(HEADER_SRC) $(INCL_DEF_SRC) $(SERV_PROX_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar los archivos generados
clean:
	@echo "Limpiando los archivos generados"
	rm -f *.o $(SERVER_EXEC) $(CLIENT_EXEC1) $(CLIENT_EXEC2) $(CLIENT_EXEC3) $(CLIENT_EXEC4) $(LIBRARY_NAME)

.PHONY: all clean