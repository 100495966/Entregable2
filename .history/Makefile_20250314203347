# Variables de compilación
CC = gcc
CFLAGS = -Wall -fPIC -g
LDFLAGS = -lrt -lpthread
LIBRARY_NAME = libclaves.so
SERVER_EXEC = servidor-mq
CLIENT_EXEC1 = app-cliente-1
CLIENT_EXEC2 = app-cliente-2
CLIENT_EXEC3 = app-cliente-3

# Directorios
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Archivos fuente
CLAVES_SRC = $(SRC_DIR)/claves.c
SERVER_SRC = $(SRC_DIR)/servidor-mq.c
CLIENT_SRC1 = $(SRC_DIR)/app-cliente-1.c
CLIENT_SRC2 = $(SRC_DIR)/app-cliente-2.c
CLIENT_SRC3 = $(SRC_DIR)/app-cliente-3.c
PROXY_SRC = $(SRC_DIR)/proxy-mq.c
HEADER_SRC = $(SRC_DIR)/claves.h

# Archivos objeto
CLAVES_OBJ = $(OBJ_DIR)/claves.o
SERVER_OBJ = $(OBJ_DIR)/servidor-mq.o
CLIENT_OBJ1 = $(OBJ_DIR)/app-cliente-1.o
CLIENT_OBJ2 = $(OBJ_DIR)/app-cliente-2.o
CLIENT_OBJ3 = $(OBJ_DIR)/app-cliente-3.o
PROXY_OBJ = $(OBJ_DIR)/proxy-mq.o

# Regla principal
all: $(LIBRARY_NAME) $(SERVER_EXEC) $(CLIENT_EXEC1) $(CLIENT_EXEC2) $(CLIENT_EXEC3)

# Crear la biblioteca dinámica
$(LIBRARY_NAME): $(PROXY_OBJ)
	@echo "Creando la biblioteca dinámica $(LIBRARY_NAME)"
	@mkdir -p $(BIN_DIR)
	$(CC) -shared -o $(LIBRARY_NAME) $(PROXY_OBJ) $(LDFLAGS)


$(SERVER_EXEC): $(SERVER_OBJ) $(CLAVES_OBJ)
	@mkdir -p $(BIN_DIR)
	@echo "Compilando el servidor $(SERVER_EXEC)"
	$(CC) -o $(BIN_DIR)/$(SERVER_EXEC) $(SERVER_OBJ) $(CLAVES_OBJ) $(LDFLAGS)

# Compilar los clientes
$(CLIENT_EXEC1): $(CLIENT_OBJ1) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC1)"
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(CLIENT_EXEC1) $(CLIENT_OBJ1) -L. -lclaves $(LDFLAGS) -Wl,-rpath,.

$(CLIENT_EXEC2): $(CLIENT_OBJ2) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC2)"
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(CLIENT_EXEC2) $(CLIENT_OBJ2) -L. -lclaves $(LDFLAGS) -Wl,-rpath,.

$(CLIENT_EXEC3): $(CLIENT_OBJ3) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC3)"
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(CLIENT_EXEC3) $(CLIENT_OBJ3) -L. -lclaves $(LDFLAGS) -Wl,-rpath,.

# Compilación de los archivos .c a .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar los archivos generados
clean:
	@echo "Limpiando los archivos generados"
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIBRARY_NAME)

# Eliminar todo, incluyendo los archivos generados
distclean: clean
	@echo "Limpiando todo"
	rm -f $(BIN_DIR)/$(SERVER_EXEC) $(BIN_DIR)/$(CLIENT_EXEC1) $(BIN_DIR)/$(CLIENT_EXEC2) $(BIN_DIR)/$(CLIENT_EXEC3) $(LIBRARY_NAME)

.PHONY: all clean distclean
