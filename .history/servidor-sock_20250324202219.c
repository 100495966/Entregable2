#include "claves.h"

//Definir mutex, variable condicional y variable global de sincronización 'busy'
pthread_mutex_t mutex2;
pthread_cond_t cond;
int busy;

//REVIEW
int strtol_handling(char * str, int * result){
    char * endPtr;
    long int n;
    errno = 0;
    n = (int) strtol(str, &endPtr, 10);
    if ((errno == ERANGE && (n == LONG_MAX || n == LONG_MIN)) || (errno != 0 && n == 0)){
        perror("La función strtol ha fallado");
        return ERROR_COMMUNICATION;
    }
    else if (endPtr == str) {
        perror("La función strtol no ha encontrado digitos");
        return ERROR_COMMUNICATION;
    }

    if (n > INT_MAX || n < INT_MIN){
        perror("La función strtol ha encontrado un valor que supera el límite de un entero");
        return ERROR_COMMUNICATION;
    }

    *result = (int) n;

    return 0;
}

//REVIEW
int strtod_handling(char * str, double * result){
    char ** endPtr;
    double n;
    errno = 0;
    n = strtod(str, endPtr);
    if ((errno == ERANGE && (n == DBL_MAX || n == DBL_MIN)) || (errno != 0 && n == 0)){
        perror("La función strtod ha fallado");
        return ERROR_COMMUNICATION;
    }
    else if (endPtr == str) {
        perror("La función strtod no ha encontrado digitos");
        return ERROR_COMMUNICATION;
    }

    *result = n;

    return 0;
}

void * SendResponse(void * sc){
    int s_local;

    pthread_mutex_lock(&mutex2);
    s_local = (* (int *) sc);
    busy = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex2);

    char action;
    int key;
    char * value1;
    int N_value2;
    double * V_value2;
    struct Coord value3;

    char buffer[256];
    int ret;

    // Recibir la acción a realizar
    if ((ret = recvMessage(s_local, &action, 1)) != 0){
        pthread_exit(ret);
    }

    printf("Solicitud recibida: Acción %c \n", action);

    // Procesar la solicitud
    switch (action) {
        case DESTROY:
            ret = destroy();
            break;
        case DELETE_KEY:
        case EXIST:
        case GET_VALUE:
            // Recibir la key
            if ((ret = readLine(s_local, buffer, 256)) != 0) goto cleanup;
            if ((ret = strtol_handling(buffer, &key)) != 0) goto cleanup;

            switch (action){
                case DELETE_KEY:
                    ret = delete_key(key);
                    break;
                case EXIST:
                    ret = exist(key);
                    break;
                case GET_VALUE:
                    //todo
                    ret = get_value(key, value1, &N_value2, V_value2, &value3);

                    // Enviar resultado
                    snprintf(buffer, sizeof(buffer), "%d", ret);
                    if ((ret = sendMessage(s_local, buffer, strlen(buffer) + 1)) != 0) goto cleanup;



                    break;
            }
            break;
        case SET_VALUE:
        case MODIFY_VALUE:
            // Recibir la key
            if ((ret = readLine(s_local, buffer, 256)) != 0) goto cleanup;
            if ((ret = strtol_handling(buffer, &key)) != 0) goto cleanup;

            // Recibir value1
            if ((value1 = (char *) malloc(256)) == NULL)  goto cleanup;
            if ((ret = readLine(s_local, value1, 256)) != 0)  goto cleanup;

            // Recibir número de doubles contenidos en value2
            if ((ret = readLine(s_local, buffer, 256)) != 0)  goto cleanup;
            if ((ret = strtol_handling(buffer, &N_value2)) != 0)  goto cleanup;

            // Recibir double a double de value2
            if ((V_value2 = (double *) malloc(N_value2 * sizeof(double))) == NULL)  goto cleanup;
            for (int i = 0; i < N_value2; i++){
                if ((ret = readLine(s_local, buffer, 256)) != 0){
                    goto cleanup;
                }
                if ((ret = strtod_handling(buffer, &V_value2[i])) != 0){
                    goto cleanup;
                }
            }

            // Recibir value3
            if ((ret = readLine(s_local, buffer, 256)) != 0)  goto cleanup;
            if ((ret = strtol_handling(buffer, &value3.x)) != 0)  goto cleanup;
            if ((ret = readLine(s_local, buffer, 256)) != 0)  goto cleanup;
            if ((ret = strtol_handling(buffer, &value3.y)) != 0)  goto cleanup;

            switch (action){
                case SET_VALUE:
                    ret = set_value(key, value1, N_value2, V_value2, value3);
                    break;
                case MODIFY_VALUE:
                    ret = modify_value(key, value1, N_value2, V_value2, value3);
                    break;
            }
            break;
        default:
            perror("SERVIDOR: No existe la acción requerida");
            ret = ERROR_COMMUNICATION;
            goto cleanup;
    }

    snprintf(buffer, sizeof(buffer), "%d", ret);
    if ((ret = sendMessage(s_local, buffer, strlen(buffer) + 1)) != 0) goto cleanup;

    ret = 0;

    cleanup:
        if (value1) free(value1);
        if (V_value2) free(V_value2);
        close(s_local);
        pthread_exit(ret);
}


int main(int argc, char * argv) {

    if (argc != 2){
        printf ("SERVIDOR. Uso: ./servidor <puerto>\n");
        return ERROR_COMMUNICATION;
    }
    //IDEA: restringir el tipo de puerto
    else{
        char *endptr;
        long port = strtol(argv[1], &endptr, 10);
        if (*endptr != '\0' || port < 1024 || port > 49151) {
            perror("SERVIDOR: debe usar un puerto registrado\n");
            return ERROR_COMMUNICATION;
        }
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int ss, sc;

    if ((ss = socket (AF_INET, SOCK_STREAM, 0) ) < 0) { 
        perror("SERVIDOR: Error en el socket\n");
        return ERROR_COMMUNICATION;
    }
    int val = 1;
    if (setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0){
        perror("SERVIDOR: Error al configurar el socket\n");
        return ERROR_COMMUNICATION;
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(ss, (const struct sockaddr *) &server_addr, sizeof(server_addr)) != 0){
        perror("SERVIDOR: Error al asignar dirección al socket\n");
        return ERROR_COMMUNICATION;
    }

    if (listen(ss, SOMAXCONN) != 0){
        perror("SERVIDOR: Error al habilitar el socket para recibir conexiones\n");
        return ERROR_COMMUNICATION;
    }
    printf("SERVIDOR: Activo\n");

    // Inicializar variable global de control (busy)
    busy = 1;

    // Inicializar mutex y variable condicional
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&cond, NULL);

    // Crear atributo de pthread DETACHED    
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);   

    size = sizeof(client_addr);

    // Bucle infinito para manejar las solicitudes
    while (1) {
        printf("SERVIDOR: Esperando conexión\n");
        if ((sc = accept(ss, &client_addr, &size)) < 0){
            perror("SERVIDOR: Error al tratar de aceptar conexión\n");
            return ERROR_COMMUNICATION;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, &thread_attr, SendResponse, (void*) &sc) != 0) {
            perror("SERVIDOR: Error al crear el hilo\n");
            return ERROR_COMMUNICATION;
        }

        pthread_mutex_lock(&mutex2);
        while(busy == 1){
            pthread_cond_wait(&cond, &mutex2);
        }
        busy = 1;
        pthread_mutex_unlock(&mutex2);

    }
    
    //Liberar recursos relacionados con el mutex
    pthread_attr_destroy(&thread_attr);
    pthread_mutex_destroy(&mutex2);

    // Cerrar el socket del servidor
    close(ss);

    return 0;
}

