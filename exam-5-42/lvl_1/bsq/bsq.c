#include <stdio.h>
#include <stdlib.h>

/* Devuelve el mínimo de tres valores */
int min3(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/* Libera la memoria del mapa 2D */
void freeMap(char **map, int filas) {
    if (!map) return;
    for (int i = 0; i < filas; i++) free(map[i]);
    free(map);
}

/* Lee y valida el mapa desde un archivo o stdin */
char **leer(FILE *fp, int *filas, int *columnas, char *vacio, char *obstaculo, char *relleno) {
    /* Cabecera: número de líneas y caracteres */
    if (fscanf(fp, "%d %c %c %c\n", filas, vacio, obstaculo, relleno) != 4 || *filas <= 0)
        return NULL;

    char **mapa = malloc(*filas * sizeof(char *));
    if (!mapa) return NULL;

    int ancho = 0;
    for (int i = 0; i < *filas; i++) {
        char *linea = NULL;
        size_t len = 0;
        ssize_t r = getline(&linea, &len, fp);

        /* Línea vacía o sin salto de línea → error */
        if (r <= 0 || linea[r - 1] != '\n') {
            free(linea);
            freeMap(mapa, i);
            return NULL;
        }
        linea[--r] = '\0'; /* Elimina el '\n' final */

        /* Verifica ancho constante entre filas */
        if (ancho == 0) ancho = r;
        else if (r != ancho) {
            free(linea);
            freeMap(mapa, i);
            return NULL;
        }

        /* Verifica caracteres válidos */
        for (int j = 0; j < r; j++) {
            if (linea[j] != *vacio && linea[j] != *obstaculo) {
                free(linea);
                freeMap(mapa, i);
                return NULL;
            }
        }
        mapa[i] = linea;
    }

    *columnas = ancho;
    return mapa;
}

/* Aplica el algoritmo del cuadrado más grande */
void resolver(char **mapa, int filas, int columnas, char vacio, char obstaculo, char relleno) {
    int **dp = calloc(filas, sizeof(int *));
    for (int i = 0; i < filas; i++) dp[i] = calloc(columnas, sizeof(int));

    int mejorTam = 0, mejorFila = 0, mejorCol = 0;

    /* Recorre el mapa aplicando DP */
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            if (mapa[i][j] == vacio) {
                if (i == 0 || j == 0)
                    dp[i][j] = 1;
                else
                    dp[i][j] = 1 + min3(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);

                /* Guarda la posición del cuadrado máximo */
                if (dp[i][j] > mejorTam) {
                    mejorTam = dp[i][j];
                    mejorFila = i;
                    mejorCol = j;
                }
            }
        }
    }

    /* Rellena el cuadrado máximo con el carácter 'relleno' */
    for (int i = mejorFila - mejorTam + 1; i <= mejorFila; i++)
        for (int j = mejorCol - mejorTam + 1; j <= mejorCol; j++)
            mapa[i][j] = relleno;

    /* Imprime el mapa final */
    for (int i = 0; i < filas; i++) {
        fputs(mapa[i], stdout);
        fputc('\n', stdout);
        free(dp[i]);
    }
    free(dp);
}

/* Procesa un archivo o la entrada estándar */
void procesar(FILE *fp) {
    int filas = 0, columnas = 0;
    char vacio = 0, obstaculo = 0, relleno = 0;

    /* Leer y validar el mapa */
    char **mapa = leer(fp, &filas, &columnas, &vacio, &obstaculo, &relleno);
    if (!mapa) {
        fprintf(stderr, "map error\n");
        return;
    }

    /* Resolver y liberar memoria */
    resolver(mapa, filas, columnas, vacio, obstaculo, relleno);
    freeMap(mapa, filas);
}

/* Programa principal */
int main(int argc, char **argv) {
    if (argc == 1) {
        procesar(stdin);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) {
                fprintf(stderr, "map error\n");
                continue;
            }
            procesar(fp);
            fclose(fp);
        }
    }
    return 0;
}
