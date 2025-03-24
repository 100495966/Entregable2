#include "claves.h"

//Definir mutex, variable condicional y variable global de sincronización 'busy'
pthread_mutex_t mutex2;
pthread_cond_t cond;
int busy;

//REVIEW
/*
int strtod_handling(char * str, double * result){
    char ** endPtr;
    double n;
    errno = 0;
    n = strtod(str, endPtr);
    if ((errno == ERANGE && (n == DBL_MAX || n == DBL_MIN)) || (errno != 0 && n == 0)){
        perror("La función strtod ha fallado");
        return -2;
    }
    else if (endPtr == str) {
        perror("La función strtod no ha encontrado digitos");
        return -2;
    }

    *result = n;

    return 0;
}
*/

void * SendResponse(void * sc){
    int s_local;

    pthread_mutex_lock(&mutex2);
    s_local = (* (int *) sc);
    busy = 0;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex2);

    char action;
    int * key;
    char * value1;
    int * N_value2;
    double * V_value2;
    struct Coord * value3;

    char buffer[256];
    int ret1;
    int ret2;

    // Recibir la acción a realizar
    if ((ret1 = recvMessage(s_local, &action, 1)) != 0){
        pthread_exit(ret1);
    }

    printf("Solicitud recibida: Acción %c \n", action);

    // Procesar la solicitud
    switch (action) {
        case DESTROY:
            ret2 = destroy();
            break;
        case DELETE_KEY:
        case EXIST:
            // Recibir la key
            if ((ret1 = readLine(s_local, buffer, 256)) != 0){
                pthread_exit(ret1);
            }
            key = (int *) strtoull(buffer, NULL, 0);

            ret2 = delete_key(*key);
            break;
        case SET_VALUE:
        case GET_VALUE:
        case MODIFY_VALUE:
            // Recibir la key
            if ((ret1 = readLine(s_local, buffer, 256)) != 0){
                pthread_exit(ret1);
            }
            key = (int *) strtoull(buffer, NULL, 0);

            // Recibir value1
            if ((ret1 = readLine(s_local, buffer, 256)) != 0){
                pthread_exit(ret1);
            }
            strcpy(value1, buffer);

            // Recibir número de doubles contenidos en value2
            if ((ret1 = readLine(s_local, buffer, 256)) != 0){
                pthread_exit(ret1);
            }
            if (ret1 = strtol_handling(buffer, N_value2) != 0){
                pthread_exit(ret1);
            }
            N_value2 = (int *) strtoull(buffer, NULL, 0);

            // Recibir double a double de value2
            if ((ret1 = readLine(s_local, buffer, 256)) != 0){
                pthread_exit(ret1);
            }
            V_value2 = (double *) strtoull(buffer, NULL, 0);

            // Recibir value3
            
            if ((ret1 = readLine(s_local, buffer, 256)) != 0){
                pthread_exit(ret1);
            }
            value3 = (struct Coord *) strtoull(buffer, NULL, 16);

            switch (action){
                case SET_VALUE:
                    ret2 = set_value(*key, value1, *N_value2, V_value2, *value3);
                    break;
                case GET_VALUE:
                    ret2 = get_value(*key, value1, N_value2, V_value2, value3);
                    break;
                case MODIFY_VALUE:
                    ret2 = modify_value(*key, value1, *N_value2, V_value2, *value3);
                    break;
            }
            break;
        default:
            perror("SERVIDOR: No existe la acción requerida");
            pthread_exit(-2);
    }

    snprintf(buffer, sizeof(buffer), "%d", ret2);
    if ((ret1 = sendMessage(s_local, buffer, strlen(buffer) + 1)) != 0){
        pthread_exit(ret1);
    }

    close(s_local);
    pthread_exit(0);
}


int main(int argc, char * argv) {

    if (argc != 2){
        printf ("SERVIDOR. Uso: ./servidor <puerto>\n");
        return -2;
    }
    //IDEA: restringir el tipo de puerto
    else if (atoi(argv[1]) < 1024 || atoi(argv[1]) > 49151){
        perror("SERVIDOR: debe usar un puerto registrado\n");
        return -2;
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int ss, sc;

    if ((ss = socket (AF_INET, SOCK_STREAM, 0) ) < 0) { 
        perror("SERVIDOR: Error en el socket\n");
        return -2;
    }
    int val = 1;
    if (setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0){
        perror("SERVIDOR: Error al configurar el socket\n");
        return -2;
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(ss, (const struct sockaddr *) &server_addr, sizeof(server_addr)) != 0){
        perror("SERVIDOR: Error al asignar dirección al socket\n");
        return -2;
    }

    if (listen(ss, SOMAXCONN) != 0){
        perror("SERVIDOR: Error al habilitar el socket para recibir conexiones\n");
        return -2;
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
            return -2;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, &thread_attr, SendResponse, (void*) &sc) != 0) {
            perror("SERVIDOR: Error al crear el hilo\n");
            return -2;
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

