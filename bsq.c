#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

int procesar(FILE *archivo) {
    char *linea = NULL;
    size_t tam = 0;
    ssize_t largo = getline(&linea, &tam, archivo);
    
    // Extraer filas
    int filas = 0, i = 0;
    while (linea[i] >= '0' && linea[i] <= '9') 
        filas = filas * 10 + (linea[i++] - '0');
    
    // Validar primera línea
    if (largo < 4 || linea[largo-1] != '\n' || largo - i != 4 || filas <= 0) {
        free(linea);
        return 0;
    }
    
    char vacio = linea[largo-4];
    char obstaculo = linea[largo-3];
    char lleno = linea[largo-2];
    free(linea);
    
    if (vacio == obstaculo || vacio == lleno || obstaculo == lleno) 
        return 0;
    
    // Leer primera fila
    linea = NULL;
    largo = getline(&linea, &tam, archivo);
    if (largo <= 1 || linea[largo-1] != '\n') {
        free(linea);
        return 0;
    }
    
    int columnas = 0;
    while (linea[columnas] != '\n') {
        if (linea[columnas] != vacio && linea[columnas] != obstaculo) {
            free(linea);
            return 0;
        }
        columnas++;
    }
    
    if (columnas == 0) {
        free(linea);
        return 0;
    }
    
    // Crear mapa
    char *mapa = malloc(filas * columnas);
    int *tabla = calloc(filas * columnas, sizeof(int));
    if (!mapa || !tabla) {
        free(linea);
        free(mapa);
        free(tabla);
        return 0;
    }
    
    for (int c = 0; c < columnas; c++) 
        mapa[c] = linea[c];
    
    // Leer resto de filas
    for (int fila = 1; fila < filas; fila++) {
        largo = getline(&linea, &tam, archivo);
        if (largo != columnas + 1 || linea[largo-1] != '\n') {
            free(linea);
            free(mapa);
            free(tabla);
            return 0;
        }
        
        for (int col = 0; col < columnas; col++) {
            if (linea[col] != vacio && linea[col] != obstaculo) {
                free(linea);
                free(mapa);
                free(tabla);
                return 0;
            }
            mapa[fila * columnas + col] = linea[col];
        }
    }
    
    free(linea);
    
    // Algoritmo DP
    int max_tamaño = 0, max_fila = 0, max_col = 0;
    
    for (int fila = 0; fila < filas; fila++) {
        for (int col = 0; col < columnas; col++) {
            int pos = fila * columnas + col;
            
            if (mapa[pos] == obstaculo) {
                tabla[pos] = 0;
            } else if (fila == 0 || col == 0) {
                tabla[pos] = 1;
            } else {
                tabla[pos] = 1 + min3(tabla[pos - columnas], 
                                      tabla[pos - 1], 
                                      tabla[pos - columnas - 1]);
            }
            
            if (tabla[pos] > max_tamaño) {
                max_tamaño = tabla[pos];
                max_fila = fila;
                max_col = col;
            }
        }
    }
    
    free(tabla);
    
    // Rellenar cuadrado
    for (int fila = max_fila - max_tamaño + 1; fila <= max_fila; fila++) {
        for (int col = max_col - max_tamaño + 1; col <= max_col; col++) {
            mapa[fila * columnas + col] = lleno;
        }
    }
    
    // Imprimir
    for (int fila = 0; fila < filas; fila++) {
        for (int col = 0; col < columnas; col++) {
            fputc(mapa[fila * columnas + col], stdout);
        }
        fputc('\n', stdout);
    }
    
    free(mapa);
    return 1;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        if (!procesar(stdin)) {
            fprintf(stderr, "map error\n");
        }
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *archivo = fopen(argv[i], "r");
            
            if (!archivo || !procesar(archivo)) {
                fprintf(stderr, "map error\n");
            }
            
            if (archivo) 
                fclose(archivo);
            
            if (i + 1 < argc) 
                fputc('\n', stdout);
        }
    }
    
    return 0;
}