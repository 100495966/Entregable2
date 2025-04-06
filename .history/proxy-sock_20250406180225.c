#include "claves.h"
#include "servidor_y_proxy.h"

//Esta función se encarga de conectar al cliente con el servidor
int connectToServer(int * s){
    char * ip_tuplas, *port_tuplas;
    //Lee las variables de su entorno
    if ((ip_tuplas = getenv("IP_TUPLAS")) == NULL){
        perror("Variable de entorno \"IP_TUPLAS\" no ha sido definida\n");
        return ERROR_COMMUNICATION;
    }
    if ((port_tuplas = getenv("PORT_TUPLAS")) == NULL){
        perror("Variable de entorno \"PORT_TUPLAS\" no ha sido definida\n");
        return ERROR_COMMUNICATION;
    }

    printf("CLIENTE: Conectando a IP: %s, Puerto: %s\n", ip_tuplas, port_tuplas);
    
    struct sockaddr_in server_addr;
    struct hostent * hp;

    // Crea el socket tipo TCP
    if ((*s = socket (AF_INET, SOCK_STREAM, 0) ) < 0) { 
        perror("CLIENTE: Error en el socket");
        return ERROR_COMMUNICATION;
    }
    
    // Inicializa la estructura de dirección del servidor en cero
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port_tuplas));

    // Obtener la dirección IP del host a partir del nombre

    if ((hp = gethostbyname(ip_tuplas)) == NULL) {
        perror("CLIENTE: Error al resolver el nombre de host");
        close(*s);
        return ERROR_COMMUNICATION;
    }

    // Copiar la IP obtenida en la estructura de dirección del servidor
    memcpy(&server_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

    // Intenta conectarse al servidor remoto
    if (connect(*s, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0){
        perror("CLIENTE: Error al establecer conexión con el servidor");
        // Cerrar y eliminar el socket
        close(*s);
        // Devolver el estado de la acción realizada por el servidor
        return ERROR_COMMUNICATION;
    }
    
    printf("CLIENTE: Conexión establecida exitosamente con el servidor.\n");
    return 0;
}

//La función es solo accesible dentro este archivo (solo para el cliente).
static int sendRequestToServer(
                                    char action, 
                                    int * key,
                                    char * value1,
                                    int * N_value2,
                                    double * V_value2,
                                    struct Coord * value3) {
    int sc, ret;
    char buffer[256];

    // Conectarse al servidor
    if ((ret = connectToServer(&sc)) != 0) {
        return ERROR_COMMUNICATION;
    }
    
    //printf("CLIENTE: conexión exitosa con el servidor\n");

    // Enviar acción a realizar
    printf("CLIENTE: Enviando acción: %c\n", action);
    if ((ret = sendMessage(sc, &action, 1)) != 0) goto cleanup;
    
    if (key != NULL){
        // Enviar la key
        snprintf(buffer, sizeof(buffer), "%d", *key);
        if ((ret= sendMessage(sc, buffer, strlen(buffer) + 1)) != 0) goto cleanup;
    }

    if (value1 != NULL){    
        // Enviar value1
        if ((ret = sendMessage(sc, value1, strlen(value1) + 1)) != 0) goto cleanup;
    }
    
    if (N_value2 != NULL){    
        // Enviar número de doubles contenidos en value2
        snprintf(buffer, sizeof(buffer), "%d", *N_value2);
        if ((ret= sendMessage(sc, buffer, strlen(buffer) + 1)) != 0) goto cleanup;
    }

    if (V_value2 != NULL && N_value2 > 0){    
        // Enviar cada elemento de value2
        for (int i = 0; i < *N_value2; i++){
            snprintf(buffer, sizeof(buffer), "%f", V_value2[i]);
            if ((ret = sendMessage(sc, buffer, strlen(buffer) + 1)) != 0) goto cleanup;
        }
    }

    if (value3 != NULL){
        // Enviar value3
        snprintf(buffer, sizeof(buffer), "%d", (*value3).x);
        if ((ret = sendMessage(sc, buffer, strlen(buffer) + 1)) != 0) goto cleanup;
        snprintf(buffer, sizeof(buffer), "%d", (*value3).y);
        if ((ret = sendMessage(sc, buffer, strlen(buffer) + 1)) != 0) goto cleanup;
    }

    // Recibir la respuesta
    if ((ret = recvMessage(sc, buffer, 2)) != 0) goto cleanup;
    if ((strtol_handling(buffer, &ret)) != 0) goto cleanup;
    printf("CLIENTE: Respuesta del servidor: %d\n", ret);

    cleanup:
        // Cerrar y eliminar el socket
        close(sc);
        // Devolver el estado de la acción realizada por el servidor
        return ret;

}

int destroy() {
    return sendRequestToServer(
        DESTROY,                // action
        NULL,                   // sin pointer a key
        NULL,                   // sin value1
        NULL,                   // sin N_value2
        NULL,                   // sin V_value2
        NULL                    // sin value3
    );
}

int delete_key(int key) {
    return sendRequestToServer(
        DELETE_KEY,             // action
        &key,                   // pointer a key
        NULL,                   // sin value1
        NULL,                   // sin N_value2
        NULL,                   // sin V_value2
        NULL                    // sin value3
    );
}

int exist(int key) {
    return sendRequestToServer(
        EXIST,                  // action
        &key,                   // pointer a key
        NULL,                   // sin value1
        NULL,                   // sin N_value2
        NULL,                   // sin V_value2
        NULL                    // sin value3
    );
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    return sendRequestToServer(
        SET_VALUE,              // action
        &key,                   // pointer a key
        value1,                 // string value1
        &N_value2,              // pointer a N_value2
        V_value2,               // array de doubles
        &value3                 // pointer a value3
    );
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    int s, ret;
    char buffer[256];

    // Conectarse al servidor
    if ((ret = connectToServer(&s)) != 0) {
        return ERROR_COMMUNICATION;
    }
    
    // Enviar acción a realizar
    char action = GET_VALUE;
    snprintf(buffer, sizeof(buffer), "%c", action);
    if ((ret = sendMessage(s, &action, 1)) != 0) goto cleanup_get_value;
    
    // Enviar key
    snprintf(buffer, sizeof(buffer), "%d", key);
    if ((ret = sendMessage(s, buffer, strlen(buffer) + 1)) != 0) goto cleanup_get_value;
    
    int result;

    // Leer el resultado
    if ((ret = read_num_from_socket(s, buffer, &result)) != 0) goto cleanup_get_value;
    
    ret = result;

    // Si es correcto, obtener los datos
    if (result == 0) {

        // Leer value1
        if ((ret = readLine(s, value1, 256)) < 0) goto cleanup_get_value;
        
        // Leer N_value2
        if ((ret = read_num_from_socket(s, buffer, N_value2)) != 0) goto cleanup_get_value;
        
        // Leer elementos de V_value2 
        for (int i = 0; i < *N_value2; i++) {
            if ((ret = readLine(s, buffer, 256)) < 0) {
                close(s);
                return ERROR_COMMUNICATION;
            }
            if ((ret = strtod_handling(buffer, &V_value2[i])) != 0) goto cleanup_get_value;
        }

        // Leer value3.x
        if ((ret = read_num_from_socket(s, buffer, &(value3->x))) != 0) goto cleanup_get_value;


        // Leer value3.y
        if ((ret = read_num_from_socket(s, buffer, &(value3->y))) != 0) goto cleanup_get_value;

        //Imprimir valores
        printf("CLIENTE: valores recibidos\n");
    }
    
    cleanup_get_value:
        close(s);
        return ret;
}



int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    return sendRequestToServer(
        MODIFY_VALUE,           // action
        &key,                   // pointer a key
        value1,                 // string value1
        &N_value2,              // pointer a N_value2
        V_value2,               // array de doubles
        &value3                 // pointer a value3
    );
}
