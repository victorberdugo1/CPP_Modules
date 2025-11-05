#include <stdio.h>
#include <stdlib.h>

int strLen(char *s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void liberarMapa(char **mapa, int filas) {
    if (!mapa) return;
    for (int i = 0; i < filas; i++) free(mapa[i]);
    free(mapa);
}

/* leerMapa: devuelve mapa (char**) o NULL si hay error
   parámetros de salida: filas, columnas, vacio, obstaculo, relleno */
char **leerMapa(FILE *fp, int *filas, int *columnas, char *vacio, char *obstaculo, char *relleno) {
    char *linea = NULL;
    size_t capacidad = 0;
    ssize_t leidos;

    /* leer cabecera: numero de filas y tres caracteres */
    if (fscanf(fp, "%d %c %c %c\n", filas, vacio, obstaculo, relleno) != 4) return NULL;
    /* validaciones de la cabecera: filas positivas y caracteres distintos */
    if (*filas <= 0 || *vacio == *obstaculo || *vacio == *relleno || *obstaculo == *relleno) return NULL;

    char **mapa = malloc(sizeof(char*) * (*filas));
    if (!mapa) return NULL;

    *columnas = 0;

    /* leer cada línea del mapa */
    for (int i = 0; i < *filas; i++) {
        leidos = getline(&linea, &capacidad, fp);
        if (leidos <= 0) { free(linea); liberarMapa(mapa, i); return NULL; }

        /* quitar salto de línea final si existe */
        if (linea[leidos - 1] == '\n') linea[--leidos] = 0;

        int ancho = strLen(linea);
        /* línea vacía no permitida */
        if (ancho == 0) { free(linea); liberarMapa(mapa, i); return NULL; }

        /* establecer o comprobar el número de columnas */
        if (*columnas == 0) *columnas = ancho;
        else if (ancho != *columnas) { free(linea); liberarMapa(mapa, i); return NULL; }

        /* comprobar que cada caracter sea válido (vacío u obstáculo) */
        for (int j = 0; j < ancho; j++) {
            if (linea[j] != *vacio && linea[j] != *obstaculo) { free(linea); liberarMapa(mapa, i); return NULL; }
        }

        mapa[i] = linea;
        linea = NULL;
        capacidad = 0;
    }

    return mapa;
}

/* mínimo de tres enteros */
int min3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

/* resolver: calcula el mayor cuadrado y lo marca con relleno */
void resolver(char **mapa, int filas, int columnas, char vacio, char obstaculo, char relleno) {
    /* arrays DP por fila: filaAnterior y filaActual */
    int *filaAnterior = calloc(columnas, sizeof(int));
    int *filaActual = calloc(columnas, sizeof(int));
    if (!filaAnterior || !filaActual) { free(filaAnterior); free(filaActual); return; }

    int mejorTam = 0;
    int mejorFila = 0;
    int mejorColumna = 0;

    /* recorre cada celda para construir la DP */
    for (int fila = 0; fila < filas; fila++) {
        for (int col = 0; col < columnas; col++) {
            /* si es obstáculo, el tamaño de cuadrado termina en 0 */
            if (mapa[fila][col] == obstaculo) filaActual[col] = 0;
            else {
                /* en la primera fila o columna el mayor cuadrado es 1 si es vacío */
                if (fila == 0 || col == 0) filaActual[col] = 1;
                else filaActual[col] = 1 + min3(filaAnterior[col], filaActual[col - 1], filaAnterior[col - 1]);

                /* actualizar mejor cuadrado y su posición (arriba-izquierda) */
                if (filaActual[col] > mejorTam) {
                    mejorTam = filaActual[col];
                    mejorFila = fila - mejorTam + 1;
                    mejorColumna = col - mejorTam + 1;
                } else if (filaActual[col] == mejorTam && mejorTam > 0) {
                    /* en caso de empate, elegir el más alto y si hay empate el más a la izquierda */
                    int filCand = fila - filaActual[col] + 1;
                    int colCand = col - filaActual[col] + 1;
                    if (filCand < mejorFila || (filCand == mejorFila && colCand < mejorColumna)) {
                        mejorFila = filCand;
                        mejorColumna = colCand;
                    }
                }
            }
        }
        /* preparar para la siguiente fila: copiar filaActual a filaAnterior y resetear filaActual */
        for (int k = 0; k < columnas; k++) {
            filaAnterior[k] = filaActual[k];
            filaActual[k] = 0;
        }
    }

    free(filaAnterior);
    free(filaActual);

    /* marcar el mayor cuadrado encontrado con el caracter de relleno */
    for (int r = mejorFila; r < mejorFila + mejorTam; r++)
        for (int c = mejorColumna; c < mejorColumna + mejorTam; c++)
            mapa[r][c] = relleno;

    /* imprimir mapa resultante */
    for (int r = 0; r < filas; r++) {
        fputs(mapa[r], stdout);
        fputs("\n", stdout);
    }
}

/* procesar archivo: lectura, resolución y limpieza */
void procesarArchivo(FILE *fp) {
    int filas = 0;
    int columnas = 0;
    char vacio = 0, obstaculo = 0, relleno = 0;

    char **mapa = leerMapa(fp, &filas, &columnas, &vacio, &obstaculo, &relleno);
    if (!mapa) { fprintf(stderr, "map error\n"); return; }

    resolver(mapa, filas, columnas, vacio, obstaculo, relleno);
    liberarMapa(mapa, filas);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        procesarArchivo(stdin);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) fprintf(stderr, "map error\n");
            else { procesarArchivo(fp); fclose(fp); }
            if (i < argc - 1) fputs("\n", stdout);
        }
    }
    return 0;
}
