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

int main() {

    sleep(2);
  
    // Insertar valores de prueba
    printf("\nCliente2: Insertando claves...\n");

    struct Coord coord1 = {2, 3};
    double vec1[3] = {5.2, 3.5, 7.4};
    
    if (set_value(5, "Ejemplo2", 3, vec1, coord1) == 0) {
        printf("Cliente2: Clave 5 insertada correctamente.\n");
    } else {
        printf("Cliente2: Error al insertar la clave 5.\n");
    }

    // Comprobar si existe una clave que creó otro cliente
    printf("\nCliente2: Verificando si la clave 41 existe...\n");
    if (exist(41) == 1) {
        printf("Cliente2: La clave 41 existe en la base de datos.\n");
    } else {
        printf("Cliente2: La clave 41 no fue encontrada.\n");
    }


    // Obtener valores almacenados de una clave creada por otro usuario
    printf("\nCliente2: Obteniendo valores de la clave 41...\n");
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;

    if ((get_value(41, value1, &N_value2, V_value2, &value3)) == 0) {
        printf("Cliente2: Valores obtenidos correctamente:\n");
        printf("    value1: %s\n", value1);
        printf("    N_value2: %d\n", N_value2);
        printf("    V_value2: ");
        print_vector(V_value2, N_value2);
        printf("    value3: (%d, %d)\n", value3.x, value3.y);
    } else {
        printf("Cliente2: Error al obtener la clave 41.\n");
    }
    
    // Modificar valores de la clave de otro usuario
    printf("\nCliente2: Modificando la clave 41...\n");
    struct Coord coord2 = {3, 4};
    double vec2[2] = {99.9, 88.8};

    if (modify_value(41, "ModificaciónExterna", 2, vec2, coord2) == 0) {
        printf(" Clave 41 modificada correctamente.\n");
    } else {
        printf("Cliente2: Error al modificar la clave 41.\n");
    }

    // Obtener los valores modificados
    printf("\nCliente2: Obteniendo valores modificados de la clave 41...\n");
    if (get_value(41, value1, &N_value2, V_value2, &value3) == 0) {
        printf("Valores obtenidos correctamente:\n");
        printf("   🔹 value1: %s\n", value1);
        printf("   🔹 N_value2: %d\n", N_value2);
        printf("   🔹 V_value2: ");
        print_vector(V_value2, N_value2);
        printf("   🔹 value3: (%d, %d)\n", value3.x, value3.y);
    } else {
        printf(" Error al obtener la clave 41.\n");
    }

    // Eliminar la clave creada por otro cliente
    printf("\nCliente2: Eliminando la clave 41...\n");
    if (delete_key(41) == 0) {
        printf("Cliente2: Clave 41 eliminada correctamente.\n");
    } else {
        printf("Cliente2: Error al eliminar la clave 41.\n");
    }

    // Verificar si la clave sigue existiendo
    printf("\nCliente2: Verificando si la clave 41 aún existe...\n");
    if (exist(41) == 1) {
        printf("Cliente2: La clave 41 aún existe (error).\n");
    } else {
        printf("Cliente2: La clave 41 ha sido eliminada correctamente.\n");
    }

    printf("\n Cliente2: Pruebas finalizadas.\n");
    return 0;
}