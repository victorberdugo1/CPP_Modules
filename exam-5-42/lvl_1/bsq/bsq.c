#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

void freeMap(char **map, int rows) {
    if (!map) return;
    for (int i = 0; i < rows; i++) {
        free(map[i]);
    }
    free(map);
}

char **leer(FILE *fp, int *rows, int *cols, char *empty, char *obs, char *full) {
    /* Leer encabezado y consumir UN caracter extra (%*c) — normalmente el '\n' */
    if (fscanf(fp, "%d %c %c %c%*c", rows, empty, obs, full) != 4 || *rows <= 0)
        return NULL;

    /* Validar que los tres caracteres sean distintos */
    if (*empty == *obs || *empty == *full || *obs == *full)
        return NULL;

    char **map = malloc((*rows) * sizeof(char *));
    if (!map) return NULL;

    int width = 0;
    for (int r = 0; r < *rows; r++) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read = getline(&line, &len, fp);
        if (read <= 0) { free(line); freeMap(map, r); return NULL; }
        if (line[read - 1] != '\n') { free(line); freeMap(map, r); return NULL; }
        line[--read] = '\0'; /* quitar '\n' */
        if (width == 0) {
            if (read == 0) { free(line); freeMap(map, r); return NULL; } /* ancho 0 no válido */
            width = read;
        } else if (read != width) {
            free(line); freeMap(map, r); return NULL;
        }
        for (int j = 0; j < read; j++) {
            if (line[j] != *empty && line[j] != *obs) { free(line); freeMap(map, r); return NULL; }
        }
        map[r] = line;
    }
    *cols = width;
    return map;
}

void resolver(char **map, int rows, int cols, char empty, char obs, char full) {
    int **dp = calloc(rows, sizeof(int *));
    if (!dp) {
        fprintf(stderr, "map error\n");
        return;
    }
    for (int i = 0; i < rows; i++) {
        dp[i] = calloc(cols, sizeof(int));
        if (!dp[i]) {
            for (int k = 0; k < i; k++) free(dp[k]);
            free(dp);
            fprintf(stderr, "map error\n");
            return;
        }
    }

    int maxSize = 0, maxRow = 0, maxCol = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (map[i][j] == empty) {
                if (i == 0 || j == 0) dp[i][j] = 1;
                else dp[i][j] = 1 + min3(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);
                if (dp[i][j] > maxSize) { maxSize = dp[i][j]; maxRow = i; maxCol = j; }
            } else {
                dp[i][j] = 0;
            }
        }
    }

    if (maxSize > 0) {
        for (int i = maxRow - maxSize + 1; i <= maxRow; i++)
            for (int j = maxCol - maxSize + 1; j <= maxCol; j++)
                map[i][j] = full;
    }

    for (int i = 0; i < rows; i++) {
        fputs(map[i], stdout);
        fputc('\n', stdout);
        free(dp[i]);
    }
    free(dp);
}

void procesar(FILE *fp) {
    int rows = 0, cols = 0;
    char empty = 0, obs = 0, full = 0;
    char **map = leer(fp, &rows, &cols, &empty, &obs, &full);
    if (!map) { fprintf(stderr, "map error\n"); return; }
    resolver(map, rows, cols, empty, obs, full);
    freeMap(map, rows);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        procesar(stdin);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) { fprintf(stderr, "map error\n"); }
            else {
                procesar(fp);
                fclose(fp);
            }
            if (i + 1 < argc)
                fputs("\n", stdout);
        }
    }
    return 0;
}
