#include <stdio.h>
#include <stdlib.h>

static int strsize(const char *s) {
    int i = 0;
    while (s && s[i]) i++;
    return i;
}

static void freemap(char **mapa, int filas) {
    if (!mapa) return;
    for (int i = 0; i < filas; i++)
        free(mapa[i]);
    free(mapa);
}

static int leer(FILE *fp, char ***mapa, int *filas, int *cols,
                char *vacio, char *bloque, char *marca) {
    char *linea = NULL;
    size_t cap = 0;
    ssize_t n;
    if (fscanf(fp, "%d %c %c %c\n", filas, vacio, bloque, marca) != 4)
        return 0;
    if (*filas <= 0 || *vacio == *bloque || *vacio == *marca || *bloque == *marca)
        return 0;
    *mapa = malloc(sizeof(char*) * (*filas));
    if (!*mapa) return 0;
    *cols = 0;
    for (int i = 0; i < *filas; i++) {
        n = getline(&linea, &cap, fp);
        if (n <= 0) {
            free(linea);
            freemap(*mapa, i);
            return 0;
        }
        if (linea[n - 1] == '\n')
            linea[--n] = '\0';
        int len = strsize(linea);
        if (len == 0) {
            free(linea);
            freemap(*mapa, i);
            return 0;
        }
        if (*cols == 0) *cols = len;
        else if (len != *cols) {
            free(linea);
            freemap(*mapa, i);
            return 0;
        }
        for (int j = 0; j < len; j++) {
            if (linea[j] != *vacio && linea[j] != *bloque) {
                free(linea);
                freemap(*mapa, i);
                return 0;
            }
        }
        (*mapa)[i] = linea;
        linea = NULL;
        cap = 0;
    }
    free(linea);
    return 1;
}

static int minimo3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

static void resolver(char **mapa, int filas, int cols, char vacio, char bloque, char marca) {
    int *anterior = calloc(cols, sizeof(int));
    int *actual = calloc(cols, sizeof(int));
    if (!anterior || !actual) {
        free(anterior);
        free(actual);
        return;
    }
    int maxlado = 0, mejorfila = 0, mejorcol = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < cols; j++) {
            if (mapa[i][j] == bloque) actual[j] = 0;
            else {
                if (i == 0 || j == 0) actual[j] = 1;
                else actual[j] = 1 + minimo3(anterior[j], actual[j - 1], anterior[j - 1]);
                if (actual[j] > maxlado) {
                    maxlado = actual[j];
                    mejorfila = i - maxlado + 1;
                    mejorcol = j - maxlado + 1;
                } else if (actual[j] == maxlado && maxlado > 0) {
                    int nuevaFila = i - actual[j] + 1;
                    int nuevaCol = j - actual[j] + 1;
                    if (nuevaFila < mejorfila || (nuevaFila == mejorfila && nuevaCol < mejorcol)) {
                        mejorfila = nuevaFila;
                        mejorcol = nuevaCol;
                    }
                }
            }
        }
        int *temp = anterior;
        anterior = actual;
        actual = temp;
        for (int k = 0; k < cols; k++)
            actual[k] = 0;
    }
    free(anterior);
    free(actual);
    for (int r = mejorfila; r < mejorfila + maxlado; r++)
        for (int c = mejorcol; c < mejorcol + maxlado; c++)
            mapa[r][c] = marca;
    for (int i = 0; i < filas; i++) {
        fputs(mapa[i], stdout);
        fputs("\n", stdout);
    }
}

static void procesar(FILE *fp) {
    char **mapa = NULL;
    int filas = 0, cols = 0;
    char vacio = 0, bloque = 0, marca = 0;
    if (!leer(fp, &mapa, &filas, &cols, &vacio, &bloque, &marca)) {
        fprintf(stderr, "map error\n");
        return;
    }
    resolver(mapa, filas, cols, vacio, bloque, marca);
    freemap(mapa, filas);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        procesar(stdin);
    } else {
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

