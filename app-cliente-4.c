#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"

#define NUM_CLAVES 1000 

// Función para generar un vector de doubles aleatorios
void generar_vector(double *vec, int n) {
    for (int i = 0; i < n; i++) {
        vec[i] = (double)(rand() % 1000) / 10.0;  // entre 0.0 y 99.9
    }
}

// Este cliente va a probar el sistema con una cantidad alta de tuplas
int main() {

    sleep(5); // Se probará este cliente por separado a los demás, para no afectar el desempeño de los otros
    printf("ClienteBulk: Insertando %d claves...\n", NUM_CLAVES);

    for (int i = 10000; i < 10000 + NUM_CLAVES; i++) {
        char value1[50];
        snprintf(value1, sizeof(value1), "Valor_%d", i);
        
        int n = 5 + rand() % 5;  // entre 5 y 9 valores
        double vec[n];
        generar_vector(vec, n);
        
        struct Coord coord = {rand() % 100, rand() % 100};

        if (set_value(i, value1, n, vec, coord) == 0) {
            printf("ClienteBulk: Clave %d insertada correctamente.\n", i);
        } else {
            printf("ClienteBulk: Error al insertar clave %d.\n", i);
        }
    }

    // Operar sobre algunas claves aleatorias
    int pruebas[] = {10010, 10050, 10100, 10011};  
    for (int j = 0; j < sizeof(pruebas)/sizeof(pruebas[0]); j++) {
        int k = pruebas[j];
        char value1[256];
        int N_value2;
        double V_value2[32];
        struct Coord value3;

        printf("\nClienteBulk: Obteniendo valores de la clave %d...\n", k);
        if (get_value(k, value1, &N_value2, V_value2, &value3) == 0) {
            printf("  value1: %s\n", value1);
            printf("  N_value2: %d\n", N_value2);
            printf("  value2: [ ");
            for (int i = 0; i < N_value2; i++) printf("%.2f ", V_value2[i]);
            printf("]\n");
            printf("  Coord: (%d, %d)\n", value3.x, value3.y);
        } else {
            printf("  Error al obtener la clave %d.\n", k);
        }

        // Modificar el valor
        printf("ClienteBulk: Modificando clave %d...\n", k);
        double new_vec[3] = {3.14, 2.71, 1.41};
        struct Coord new_coord = {42, 24};
        if (modify_value(k, "Modificado", 3, new_vec, new_coord) == 0) {
            printf("  Clave %d modificada correctamente.\n", k);
        } else {
            printf("  Error al modificar clave %d.\n", k);
        }

        // Revisar que se haya modificado
        if (get_value(k, value1, &N_value2, V_value2, &value3) == 0) {
            printf("  value1: %s\n", value1);
            printf("  N_value2: %d\n", N_value2);
            printf("  value2: [ ");
            for (int i = 0; i < N_value2; i++) printf("%.2f ", V_value2[i]);
            printf("]\n");
            printf("  Coord: (%d, %d)\n", value3.x, value3.y);
        } else {
            printf("  Error al obtener la clave %d.\n", k);
        }
    }
    // Observar si una clave existe
    printf("\nClienteBulk: Verificando si la clave 10030 existe...\n");
    if (exist(10030) == 1) {
        printf("ClienteBulk: La clave 10030 existe en la base de datos.\n");
    } else {
        printf("ClienteBulk: La clave 10030 no fue encontrada.\n");
    }

    // Eliminamos clave cuando hay gran volumen
    printf("\nClienteBulk: Eliminando la clave 10030...\n");
    if (delete_key(10030) == 0) {
        printf("ClienteBulk: Clave 10030 eliminada correctamente.\n");
    } else {
        printf("ClienteBulk: Error al eliminar la clave 10030.\n");
    }

    // Revisar si sigue existiendo
    printf("\nClienteBulk: Verificando si la clave 10030 sigue existiendo...\n");
    if (exist(10030) == 1) {
        printf("ClienteBulk: La clave 10030 existe en la base de datos.\n");
    } else {
        printf("ClienteBulk: La clave 10030 no fue encontrada.\n");
    }

    // Borrar todas las claves
    printf("\nClienteBulk: Eliminando todos los datos ...\n");
    if (destroy() == 0) {
        printf("ClienteBulk:Todos los valores fueron eliminados.\n");
    } else {
        printf("ClienteBulk: Error al eliminar.\n");
    }

    // Revisar si sigue existiendo una llave del rango
    printf("\nClienteBulk: Verificando si la clave 10010 sigue existiendo...\n");
    if (exist(10010) == 1) {
        printf("ClienteBulk: La clave 10010 existe en la base de datos.\n");
    } else {
        printf("ClienteBulk: La clave 10010 no fue encontrada.\n");
    }

    printf("\nClienteBulk: Pruebas masivas finalizadas.\n");
    return 0;
}
