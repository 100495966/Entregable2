#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"


// Función para imprimir un vector de doubles
void print_vector(double *vector, int size) {
    printf("[ ");
    for (int i = 0; i < size; i++) {
        printf("%.2f ", vector[i]);
    }
    printf("]\n");
}

// Este cliente se encarga de probar las funciones en escenarios en los que deberían funcionar
int main() {

    // Eliminar todos los valores previos
    printf("\nCliente1: Eliminando datos previos...\n");
    if (destroy() == 0) {
        printf("Cliente1:Todos los valores fueron eliminados.\n");
    } else {
        printf("Cliente1: Error al eliminar.\n");
    }

    // Insertar valores de prueba, se insertan dos para probar si se logra iterar a través de las tuplas
    printf("\nCliente1: Insertando claves...\n");

    struct Coord coord1 = {2, 3};
    double vec1[3] = {5.2, 3.5, 7.4};
    
    if (set_value(43, "Ejemplo1", 3, vec1, coord1) == 0) {
        printf("Cliente1: Clave 43 insertada correctamente.\n");
    } else {
        printf("Cliente1: Error al insertar la clave 43.\n");
    }

    coord1.x = 1;
    coord1.y = 2;
    vec1[0] = 10.5; 
    vec1[1] = 20.5;
    vec1[2] = 30.5;

    if (set_value(42, "Ejemplo2", 3, vec1, coord1) == 0) {
        printf("Cliente1: Clave 42 insertada correctamente.\n");
    } else {
        printf("Cliente1: Error al insertar la clave 42.\n");
    }

    coord1.x = 9;
    coord1.y = 2;
    vec1[0] = 4.3; 
    vec1[1] = 5.7;
    vec1[2] = 2.5;

    if (set_value(41, "Ejemplo3", 3, vec1, coord1) == 0) {
        printf("Cliente1: Clave 41 insertada correctamente.\n");
    } else {
        printf("Cliente1: Error al insertar la clave 41.\n");
    }
    
    

    // Comprobar si existe la clave
    printf("\nCliente1: Verificando si la clave 42 existe...\n");
    if (exist(42) == 1) {
        printf("Cliente1: La clave 42 existe en la base de datos.\n");
    } else {
        printf("Cliente1: La clave 42 no fue encontrada.\n");
    }


    // Obtener valores almacenados
    printf("\nCliente1: Obteniendo valores de la clave 42...\n");
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
    
    if (get_value(42, value1, &N_value2, V_value2, &value3) == 0) {
        printf("Cliente1: Valores obtenidos correctamente:\n");
        printf("    value1: %s\n", value1);
        printf("    N_value2: %d\n", N_value2);
        printf("    V_value2: ");
        print_vector(V_value2, N_value2);
        printf("    value3: (%d, %d)\n", value3.x, value3.y);
    } else {
        printf("Cliente1: Error al obtener la clave 42.\n");
    }
    
    // Modificar valores de la clave
    printf("\nCliente1: Modificando la clave 42...\n");
    struct Coord coord2 = {3, 4};
    double vec2[2] = {99.9, 88.8};

    if (modify_value(42, "EjemploModificado", 2, vec2, coord2) == 0) {
        printf("Cliente1: Clave 42 modificada correctamente.\n");
    } else {
        printf("Cliente1: Error al modificar la clave 42.\n");
    }
    
    // Obtener los valores modificados
    printf("\nCliente1: Obteniendo valores modificados de la clave 42...\n");
    if (get_value(42, value1, &N_value2, V_value2, &value3) == 0) {
        printf("Cliente1: Valores obtenidos correctamente:\n");
        printf("   🔹 value1: %s\n", value1);
        printf("   🔹 N_value2: %d\n", N_value2);
        printf("   🔹 V_value2: ");
        print_vector(V_value2, N_value2);
        printf("   🔹 value3: (%d, %d)\n", value3.x, value3.y);
    } else {
        printf("Cliente1: Error al obtener la clave 42.\n");
    }

    // Eliminar la clave
    printf("\nCliente1: Eliminando la clave 42...\n");
    if (delete_key(42) == 0) {
        printf("Cliente1: Clave 42 eliminada correctamente.\n");
    } else {
        printf("Cliente1: Error al eliminar la clave 42.\n");
    }

    // Verificar si la clave sigue existiendo
    printf("\nCliente1: Verificando si la clave 42 aún existe...\n");
    if (exist(42) == 1) {
        printf("Cliente1: La clave 42 aún existe (error).\n");
    } else {
        printf("Cliente1: La clave 42 ha sido eliminada correctamente.\n");
    }

    printf("\nCliente1: Pruebas finalizadas.\n");
    return 0;
}

