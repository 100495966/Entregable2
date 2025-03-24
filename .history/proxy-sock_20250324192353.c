#include "claves.h"

//La función es solo accesible dentro este archivo (solo para el cliente).
static int send_request_to_server(
                                    char action, 
                                    int * key,
                                    char * value1,
                                    int * N_value2,
                                    double * V_value2,
                                    struct Coord * value3) {
    char * ip_tuplas, *port_tuplas;

    if ((ip_tuplas = getenv("IP_TUPLAS")) == NULL){
        perror("Variable de entorno \"IP_TUPLAS\" no ha sido definida\n");
        return -2;
    }
    if ((port_tuplas = getenv("PORT_TUPLAS")) == NULL){
        perror("Variable de entorno \"PORT_TUPLAS\" no ha sido definida\n");
        return -2;
    }

    int sc;
    struct sockaddr_in server_addr;
    struct hostent * hp;

    if ((sc = socket (AF_INET, SOCK_STREAM, 0) ) < 0) { 
        perror("CLIENTE: Error en el socket");
        return -2;
    }
    
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port_tuplas));
    hp = gethostbyname(ip_tuplas); 
    memcpy(&server_addr.sin_addr, hp->h_addr_list, hp->h_length);

    if (connect(sc, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0){
        perror("CLIENTE: Error al establecer conexión con el servidor");
        return -2;
    }
    
    char buffer[256];
    int ret1;

    // Enviar acción a realizar
    if ((ret1= sendMessage(sc, &action, 1)) != 0){
        return ret1;
    }
    
    if (key != NULL){
        // Enviar la key
        snprintf(buffer, sizeof(buffer), "%d", *key);
        if ((ret1= sendMessage(sc, buffer, strlen(buffer) + 1)) != 0){
            return ret1;
        }
    }

    if (value1 != NULL){    
        // Enviar value1
        strcpy(buffer, value1);
        if ((ret1= sendMessage(sc, buffer, strlen(buffer) + 1)) != 0){
            return ret1;
        }
    }
    
    if (N_value2 != NULL){    
        // Enviar número de doubles contenidos en value2
        snprintf(buffer, sizeof(buffer), "%d", *N_value2);
        if ((ret1= sendMessage(sc, buffer, strlen(buffer) + 1)) != 0){
            return ret1;
        }
    }

    if (V_value2 != NULL && N_value2 > 0){    
        // Enviar value2
        for (int i = 0; i < *N_value2; i++){
            snprintf(buffer, sizeof(buffer), "%f", V_value2[i]);
            if ((ret1 = sendMessage(sc, buffer, strlen(buffer) + 1)) != 0){
                return ret1;
            }
        }
        
    }

    if (value3 != NULL){
        // Enviar value3
        snprintf(buffer, sizeof(buffer), "%d", (*value3).x);
        if ((ret1= sendMessage(sc, buffer, strlen(buffer) + 1)) != 0){
            return ret1;
        }
        snprintf(buffer, sizeof(buffer), "%d", (*value3).y);
        if ((ret1= sendMessage(sc, buffer, strlen(buffer) + 1)) != 0){
            return ret1;
        }
    }

    // Recibir la respuesta
    if ((ret1= recvMessage(sc, buffer, 1)) != 0){
        return ret1;
    }

    int ret2;
    strtol_handling(buffer, &ret2);
    
    // Cerrar y eliminar el socket
    close(sc);

    return ret2;

}

int destroy() {
    return send_request_to_server(
        DESTROY,                // action
        NULL,                   // sin pointer a key
        NULL,                   // sin value1
        NULL,                   // sin N_value2
        NULL,                   // sin V_value2
        NULL                    // sin value3
    );
}

int delete_key(int key) {
    return send_request_to_server(
        DELETE_KEY,             // action
        &key,                   // pointer a key
        NULL,                   // sin value1
        NULL,                   // sin N_value2
        NULL,                   // sin V_value2
        NULL                    // sin value3
    );
}

int exist(int key) {
    return send_request_to_server(
        EXIST,                  // action
        &key,                   // pointer a key
        NULL,                   // sin value1
        NULL,                   // sin N_value2
        NULL,                   // sin V_value2
        NULL                    // sin value3
    );
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    return send_request_to_server(
        SET_VALUE,              // action
        &key,                   // pointer a key
        value1,                 // string value1
        &N_value2,              // pointer a N_value2
        V_value2,               // array de doubles
        &value3                 // pointer a value3
    );
}

//FIXME
/*
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    return send_request_to_server(
        GET_VALUE,              // action
        &key,                   // pointer a key
        value1,                 // string value1
        &N_value2,              // pointer a N_value2
        V_value2,               // array de doubles
        &value3                 // pointer a value3
    );
}
*/

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    return send_request_to_server(
        MODIFY_VALUE,              // action
        &key,                   // pointer a key
        value1,                 // string value1
        &N_value2,              // pointer a N_value2
        V_value2,               // array de doubles
        &value3                 // pointer a value3
    );
}
