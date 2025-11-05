#include <stdio.h>
#include <stdlib.h>

/* devuelve el menor de tres */
int menor3(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/* libera una matriz de cadenas */
void freemap(char **mapa, int filas) {
    if (!mapa) return;
    for (int f = 0; f < filas; f++) free(mapa[f]);
    free(mapa);
}

/* valida una línea leída: comprueba '\n', ancho consistente y caracteres válidos.
   Si la línea es válida reemplaza el '\n' por '\0' y actualiza *ancho si es la primera. */
int validar_linea(char *linea, ssize_t leidos, int *ancho, char vacio, char obst) {
    if (leidos <= 0) return 0;
    if (linea[leidos - 1] != '\n') return 0;   /* línea sin salto final -> error de formato */
    linea[--leidos] = '\0';                    /* quitar '\n' y dejar C-string */

    if (*ancho == 0) {
        *ancho = (int)leidos;                  /* primera línea determina el ancho */
    } else if ((int)leidos != *ancho) {
        return 0;                              /* ancho distinto -> error */
    }

    for (int c = 0; c < leidos; c++) {
        if (linea[c] != vacio && linea[c] != obst) return 0; /* caracter inesperado */
    }
    return 1;
}

/* lee el mapa desde fp; devuelve matriz, rellena filas y cols, y los caracteres */
char **leer(FILE *fp, int *filas, int *cols, char *vacio, char *obst, char *lleno) {
    if (fscanf(fp, "%d %c %c %c\n", filas, vacio, obst, lleno) != 4 || *filas <= 0) return NULL;
    char **mapa = malloc(*filas * sizeof(char *));
    if (!mapa) return NULL;

    int ancho = 0;
    for (int f = 0; f < *filas; f++) {
        char *linea = NULL;
        size_t len = 0;
        ssize_t leidos = getline(&linea, &len, fp);

        /* validar la línea con la función separada */
        if (!validar_linea(linea, leidos, &ancho, *vacio, *obst)) {
            free(linea);
            freemap(mapa, f);   /* liberar las líneas ya leídas */
            return NULL;
        }

        mapa[f] = linea;        /* línea válida: guardar el puntero (ya termina en '\0') */
    }
    *cols = ancho;
    return mapa;
}

/* resuelve el mayor cuadrado de 'vacio' y lo marca con 'lleno' */
void solve(char **mapa, int filas, int cols, char vacio, char obst, char lleno) {
    int **tabla = calloc(filas, sizeof(int *));
    if (!tabla) return;
    for (int i = 0; i < filas; i++) tabla[i] = calloc(cols, sizeof(int));

    int maxTam = 0, maxFila = 0, maxCol = 0;
    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < cols; c++) {
            if (mapa[f][c] == vacio) {
                if (f == 0 || c == 0) tabla[f][c] = 1;
                else tabla[f][c] = 1 + menor3(tabla[f - 1][c], tabla[f][c - 1], tabla[f - 1][c - 1]);
                if (tabla[f][c] > maxTam) {
                    maxTam = tabla[f][c];
                    maxFila = f;
                    maxCol = c;
                }
            }
        }
    }

    /* marcar el cuadrado encontrado */
    for (int f = maxFila - maxTam + 1; f <= maxFila; f++) {
        for (int c = maxCol - maxTam + 1; c <= maxCol; c++) {
            mapa[f][c] = lleno;
        }
    }

    /* imprimir y liberar tabla de DP */
    for (int f = 0; f < filas; f++) {
        fputs(mapa[f], stdout);
        fputc('\n', stdout);
        free(tabla[f]);
    }
    free(tabla);
}

/* procesa un stream (archivo) */
void proces(FILE *fp) {
    int filas = 0, cols = 0;
    char vacio = 0, obst = 0, lleno = 0;
    char **mapa = leer(fp, &filas, &cols, &vacio, &obst, &lleno);
    if (!mapa) {
        fprintf(stderr, "map error\n");
        return;
    }
    solve(mapa, filas, cols, vacio, obst, lleno);
    freemap(mapa, filas);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        proces(stdin);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) {
                fprintf(stderr, "map error\n");
                continue;
            }
            proces(fp);
            fclose(fp);
        }
    }
    return 0;
}
