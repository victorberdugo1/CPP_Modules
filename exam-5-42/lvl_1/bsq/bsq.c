#include <stdio.h>
#include <stdlib.h>

/* cuenta la longitud de una cadena */
int strLen(char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

/* libera toda la memoria del mapa */
void freeMap(char **mapa, int filas) {
    if (!mapa) return;
    for (int i = 0; i < filas; i++) free(mapa[i]);
    free(mapa);
}

/* lee el mapa del archivo y valida su formato */
char **leer(FILE *fp, int *filas, int *columnas, char *vacio, char *obstaculo, char *relleno) {
    char *linea = NULL;
    size_t cap = 0;
    ssize_t leidos;

    /* cabecera con nº de filas y caracteres especiales */
    if (fscanf(fp, "%d %c %c %c\n", filas, vacio, obstaculo, relleno) != 4) return NULL;
    if (*filas <= 0 || *vacio == *obstaculo || *vacio == *relleno || *obstaculo == *relleno) return NULL;

    char **mapa = malloc(sizeof(char*) * (*filas));
    if (!mapa) return NULL;

    *columnas = 0;

    /* lectura línea a línea del mapa */
    for (int i = 0; i < *filas; i++) {
        leidos = getline(&linea, &cap, fp);
        if (leidos <= 0) { free(linea); freeMap(mapa, i); return NULL; }

        /* quitar salto de línea al final */
        if (linea[leidos - 1] == '\n') linea[--leidos] = 0;

        int ancho = strLen(linea);
        if (ancho == 0) { free(linea); freeMap(mapa, i); return NULL; }

        /* comprobar que todas las filas tienen la misma longitud */
        if (*columnas == 0) *columnas = ancho;
        else if (ancho != *columnas) { free(linea); freeMap(mapa, i); return NULL; }

        /* validar que cada carácter sea válido */
        for (int j = 0; j < ancho; j++)
            if (linea[j] != *vacio && linea[j] != *obstaculo) { free(linea); freeMap(mapa, i); return NULL; }

        /* guardar la línea y reiniciar buffers */
        mapa[i] = linea;
        linea = NULL;
        cap = 0;
    }

    return mapa;
}

/* devuelve el menor de tres valores */
int min3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

/* resuelve el mapa buscando el mayor cuadrado vacío */
void resolver(char **mapa, int filas, int columnas, char vacio, char obstaculo, char relleno) {
    int *filaPrev = calloc(columnas, sizeof(int));
    int *filaAct = calloc(columnas, sizeof(int));
    if (!filaPrev || !filaAct) { free(filaPrev); free(filaAct); return; }

    int mejorTam = 0;   /* tamaño del cuadrado más grande encontrado */
    int mejorFila = 0;  /* coordenada fila de inicio del cuadrado */
    int mejorCol = 0;   /* coordenada columna de inicio del cuadrado */

    /* algoritmo DP:
       filaAct[c] guarda el tamaño del cuadrado que termina en (f,c) */
    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < columnas; c++) {
            if (mapa[f][c] == obstaculo) filaAct[c] = 0;
            else {
                if (f == 0 || c == 0)
                    filaAct[c] = 1; /* bordes: solo tamaño 1 */
                else
                    filaAct[c] = 1 + min3(filaPrev[c], filaAct[c - 1], filaPrev[c - 1]);

                /* si encontramos un cuadrado más grande, lo guardamos */
                if (filaAct[c] > mejorTam) {
                    mejorTam = filaAct[c];
                    mejorFila = f - mejorTam + 1;
                    mejorCol = c - mejorTam + 1;
                }
                /* desempate: prioridad arriba y luego a la izquierda */
                else if (filaAct[c] == mejorTam && mejorTam > 0) {
                    int candFila = f - filaAct[c] + 1;
                    int candCol = c - filaAct[c] + 1;
                    if (candFila < mejorFila || (candFila == mejorFila && candCol < mejorCol)) {
                        mejorFila = candFila;
                        mejorCol = candCol;
                    }
                }
            }
        }

        /* copiar fila actual a fila previa para siguiente iteración */
        for (int k = 0; k < columnas; k++) {
            filaPrev[k] = filaAct[k];
            filaAct[k] = 0;
        }
    }

    free(filaPrev);
    free(filaAct);

    /* pintar el cuadrado encontrado con el carácter de relleno */
    for (int r = mejorFila; r < mejorFila + mejorTam; r++)
        for (int c = mejorCol; c < mejorCol + mejorTam; c++)
            mapa[r][c] = relleno;

    /* imprimir el mapa final */
    for (int r = 0; r < filas; r++) {
        fputs(mapa[r], stdout);
        fputs("\n", stdout);
    }
}

/* procesa un archivo o la entrada estándar */
void procesar(FILE *fp) {
    int filas = 0, columnas = 0;
    char vacio = 0, obstaculo = 0, relleno = 0;

    /* leer y validar el mapa */
    char **mapa = leer(fp, &filas, &columnas, &vacio, &obstaculo, &relleno);
    if (!mapa) { fprintf(stderr, "map error\n"); return; }

    /* resolver y liberar memoria */
    resolver(mapa, filas, columnas, vacio, obstaculo, relleno);
    freeMap(mapa, filas);
}

/* main: si no hay argumentos, lee stdin; si hay, procesa cada archivo */
int main(int argc, char **argv) {
    if (argc == 1)
        procesar(stdin);
    else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp)
                fprintf(stderr, "map error\n");
            else {
                procesar(fp);
                fclose(fp);
            }
            if (i < argc - 1)
                fputs("\n", stdout);
        }
    }
    return 0;
}
