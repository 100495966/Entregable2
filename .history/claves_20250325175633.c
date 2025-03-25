#include <stdio.h>
#include "claves.h"
#include <stdlib.h>
#include <pthread.h>

// Inicializar el mutex y la variable condicional
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

// Definir funciones para mandar y recibir mensajes
ssize_t readLine(int socket, void *buffer, size_t n)
{
    if (n <= 0 || buffer == NULL) {
        perror("Error al leer el mensaje");
        return -2;
    }

    // Número de bytes leídos por el último fetch
    ssize_t numRead;   
    // Total de bytes leídos hasta el momento
    size_t totRead = 0;   
    char * buf = buffer;
    char ch;

    for (;;) {
        // Leer un byte
        numRead = read(socket, &ch, 1);   
        if (numRead == -1) {
            // reiniciar read al ser interrumpido
            if (errno == EINTR) continue;
            else{
                //Cualquier otro error
                close(socket);
                perror("Error al leer el mensaje");
                return -2;                  
            }
        } else if (numRead == 0) { //EOF
            // ningún byte leído, lo que implica que el archivo está vacío
            if (totRead == 0)          
                return 0;
            else
                break;
        } else { 
            // numRead debe ser 1 si hemos llegado hasta aquí
            if (ch == '\n')
                break;
            if (ch == '\0')
                break;
            // descartar n-1 bytes
            if (totRead < n - 1) {     
                totRead++;
                *buf++ = ch;
            }
        }
    }

    *buf = '\0';
    return totRead;
}

int sendMessage(int socket, char * buffer, int len){
    int r;
    int l = len;
    do {
        r = write(socket, buffer, 1);
        l -= r;
        buffer += r;
    } while ((l>0) && (r>=0));

    if (r < 0){
        close(socket);
        perror("Error al enviar la petición");
        return -2;
    }
    else {
        return 0;
    }

}

int recvMessage(int socket, char * buffer, int len){
    int r;
    int l = len;
    do {
        r = read(socket, buffer, 1);
        l -= r;
        buffer += r;
    } while ((l>0) && (r>=0));

    if (r < 0){
        close(socket);
        perror("Error al recuperar la respuesta");
        return -2;
    }
    else {
        return 0;
    }
}

//Definir funciones para convertir strings a números
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

double strtod_handling(char * str, double * result){
    char * endPtr;
    double n;
    errno = 0;
    n = strtod(str, &endPtr);
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

// Definir función para leer números desde un socket
int read_num_from_socket(int s_local, char * buffer, int* num) {
    if (readLine(s_local, buffer, 256) != 0) return ERROR_COMMUNICATION;
    if (strtol_handling(buffer, num) != 0) return ERROR_COMMUNICATION;
    return 0;
}

// Estructura para almacenar las tuplas
typedef struct Tupla {
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
    struct Tupla *next;
} Tupla;

//Puntero a la lista de tuplas
static Tupla *head  = NULL;

//Implementación de las funciones

int destroy() {
    pthread_mutex_lock(&mutex1);
    Tupla *current = head;
    while (current != NULL) {
        Tupla *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    pthread_mutex_unlock(&mutex1);

    return 0;
}

int delete_key(int key){
    pthread_mutex_lock(&mutex1);
    Tupla *current = head;
    Tupla *prev = NULL;
    while(current!=NULL){
        if(current->key == key){
            if (prev){          //Si no es el primer elemento
                prev->next = current->next;
            }
            else{
                head = current -> next;
            }
            free(current);
            pthread_mutex_unlock(&mutex1);
            return 0;
        }
        prev = current;
        current = current -> next;
    }
    pthread_mutex_unlock(&mutex1);
    return ERROR_TUPLAS;
}

int exist(int key){
    pthread_mutex_lock(&mutex1);
    Tupla *current = head;
    while(current!=NULL){
        if (current->key == key){
            pthread_mutex_unlock(&mutex1);
            return 1;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex1);
    return 0;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3){
    if (value1 == NULL || V_value2 == NULL || N_value2 > 32 || N_value2 < 1 || strnlen(value1, 256) > 255){
        return ERROR_TUPLAS;
    }
    pthread_mutex_lock(&mutex1);
    //Verificar si la llave ya existe
    Tupla *current = head;
    while (current != NULL){
        if (current->key == key){
            pthread_mutex_unlock(&mutex1);
            return ERROR_TUPLAS;
        }
        current = current -> next;
    }

    Tupla *new_tupla = malloc(sizeof(Tupla));
    if (new_tupla==NULL){
        pthread_mutex_unlock(&mutex1);
        return ERROR_TUPLAS;
    }
    
    new_tupla->key = key;
    strncpy(new_tupla->value1, value1, 255); //Efectivo cuando hay límite de tamaño; 255 + código zero
    new_tupla->value1[255] = '\0'; //Nos aseguramos que existe el \0 incluso si el valor es exacto
    new_tupla->N_value2 = N_value2;
    memcpy(new_tupla->V_value2, V_value2, N_value2 * sizeof(double)); //Más efectivo que copiar con un loop
    new_tupla->value3 = value3;
    new_tupla->next = head;
    head = new_tupla;
    pthread_mutex_unlock(&mutex1);
    return 0;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3){
    if (value1 == NULL || N_value2 == NULL || V_value2 == NULL || value3 == NULL) {
        return ERROR_TUPLAS;
    }        
    pthread_mutex_lock(&mutex1);
    Tupla *current = head;
    while (current != NULL) {
        if (current->key == key) {
            strncpy(value1, current->value1, 255);
            value1[255] = '\0';
            *N_value2 = current->N_value2;
            memcpy(V_value2, current->V_value2, (*N_value2) * sizeof(double));
            *value3 = current->value3;
            pthread_mutex_unlock(&mutex1);
            return 0;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex1);
    return ERROR_TUPLAS; 
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3){
    if (value1 == NULL || V_value2 == NULL || N_value2 > 32 || N_value2 < 1 || strnlen(value1, 256) > 255){
        return ERROR_TUPLAS;
    }
    
    pthread_mutex_lock(&mutex1);
    Tupla *current = head;
    while (current != NULL) {
        if (current->key == key) {
            strncpy(current->value1, value1, 255);
            current->N_value2 = N_value2;
            memcpy(current->V_value2, V_value2, N_value2 * sizeof(double));
            current-> value3 = value3;
            pthread_mutex_unlock(&mutex1);
            return 0;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex1);
    return ERROR_TUPLAS; 
}