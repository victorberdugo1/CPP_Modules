#include <stdio.h>
#include <stdlib.h>

/* Calcula el mínimo de tres valores */
int min3(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/* Libera la memoria de un mapa bidimensional */
void freeMap(char **map, int rows) {
    if (!map) return;
    for (int i = 0; i < rows; i++) free(map[i]);
    free(map);
}

/* Procesa un archivo y carga el mapa */
int processFile(FILE *fp, char ***mapOut, int *rows, int *cols, char *empty, char *obstacle, char *full) {
    if (fscanf(fp, "%d %c %c %c\n", rows, empty, obstacle, full) != 4 || *rows <= 0)
        return 0;
    *mapOut = malloc(*rows * sizeof(char *));
    if (!*mapOut) return 0;

    int width = 0;
    for (int r = 0; r < *rows; r++) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read = getline(&line, &len, fp);
        if (read <= 0 || line[read - 1] != '\n') {
            free(line);
            freeMap(*mapOut, r);
            return 0;
        }
        line[--read] = '\0';
        if (width == 0) width = read;
        else if (read != width) {
            free(line);
            freeMap(*mapOut, r);
            return 0;
        }
        for (int j = 0; j < read; j++)
            if (line[j] != *empty && line[j] != *obstacle) {
                free(line);
                freeMap(*mapOut, r);
                return 0;
            }
        (*mapOut)[r] = line;
    }
    *cols = width;
    return 1;
}

/* Calcula y marca el mayor cuadrado posible en el mapa */
void solveMap(char **map, int rows, int cols, char empty, char obstacle, char full) {
    int **dp = calloc(rows, sizeof(int *));
    for (int i = 0; i < rows; i++) dp[i] = calloc(cols, sizeof(int));

    int maxSize = 0, maxRow = 0, maxCol = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (map[i][j] == empty) {
                if (i == 0 || j == 0) dp[i][j] = 1;
                else dp[i][j] = min3(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]) + 1;
                if (dp[i][j] > maxSize) {
                    maxSize = dp[i][j];
                    maxRow = i;
                    maxCol = j;
                }
            }
        }
    }

    for (int i = maxRow - maxSize + 1; i <= maxRow; i++)
        for (int j = maxCol - maxSize + 1; j <= maxCol; j++)
            map[i][j] = full;

    for (int i = 0; i < rows; i++) {
        fputs(map[i], stdout);
        fputc('\n', stdout);
        free(dp[i]);
    }
    free(dp);
}

/* Programa principal */
int main(int argc, char **argv) {
    if (argc == 1) {
        char **map; int rows, cols; char e, o, f;
        if (processFile(stdin, &map, &rows, &cols, &e, &o, &f)) {
            solveMap(map, rows, cols, e, o, f);
            freeMap(map, rows);
        } else fputs("map error\n", stderr);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) { fputs("map error\n", stderr); continue; }
            char **map; int rows, cols; char e, o, f;
            if (processFile(fp, &map, &rows, &cols, &e, &o, &f)) {
                solveMap(map, rows, cols, e, o, f);
                freeMap(map, rows);
            } else fputs("map error\n", stderr);
            fclose(fp);
        }
    }
    return 0;
}
