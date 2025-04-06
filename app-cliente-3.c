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

// Este cliente se encarga de probar escenarios que deberían dar error, y prueba la concurrencia
int main() {

    //Esto debería dar un error, al estar ya subida por el otro cliente
    struct Coord coord1 = {4, 7};
    double vec1[3] = {25.2, 35.5, 37.4};
    printf("\nCliente3: Insertando claves...\n");

    if (set_value(43, "Ejemplo1", 3, vec1, coord1) == 0) {
        printf("Cliente3: Clave 43 insertada correctamente.\n");
    } else {
        printf("Cliente3: Error al insertar la clave 43.\n");
    }

    // Esto debería devolver error ya que la clave 30 no existe
    printf("\nCliente3: Verificando si la clave 30 existe...\n");
    if (exist(30) == 1) {
        printf("Cliente3: La clave 30 existe en la base de datos.\n");
    } else {
        printf("Cliente3: La clave 30 no fue encontrada.\n");
    }

    // Buscamos una clave que no existe
    printf("\nCliente3: Obteniendo valores de la clave 50...\n");
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;


    if ((get_value(50, value1, &N_value2, V_value2, &value3)) == 0) {
        printf("Cliente3: Valores obtenidos correctamente:\n");
        printf("    value1: %s\n", value1);
        printf("    N_value2: %d\n", N_value2);
        printf("    V_value2: ");
        print_vector(V_value2, N_value2);
        printf("    value3: (%d, %d)\n", value3.x, value3.y);
    } else {
        printf("Cliente3: Error al obtener la clave 50.\n");
    }
    
    // Modificar una clave no existente
    printf("\nCliente3: Modificando la clave 50...\n");
    struct Coord coord2 = {3, 4};
    double vec2[2] = {99.9, 88.8};

    if (modify_value(50, "ModificaciónExterna", 2, vec2, coord2) == 0) {
        printf("Cliente3: Clave 50 modificada correctamente.\n");
    } else {
        printf("Cliente3: Error al modificar la clave 50.\n");
    }

    // Eliminamos clave no existente
    printf("\nCliente3: Eliminando la clave 50...\n");
    if (delete_key(50) == 0) {
        printf("Cliente3: Clave 50 eliminada correctamente.\n");
    } else {
        printf("Cliente3: Error al eliminar la clave 50.\n");
    }

    printf("\nCliente3: Pruebas finalizadas.\n");
    return 0;
}