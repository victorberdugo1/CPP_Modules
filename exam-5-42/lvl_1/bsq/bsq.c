#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

void freeMap(char **map, int rows) {
    if (!map) return;
    for (int i = 0; i < rows; i++) free(map[i]);
    free(map);
}

char **leer(FILE *fp, int *rows, int *cols, char *empty, char *obs, char *full) {
    if (fscanf(fp, "%d %c %c %c\n", rows, empty, obs, full) != 4 || *rows <= 0) return NULL;
    char **map = malloc(*rows * sizeof(char *));
    if (!map) return NULL;

    int width = 0;
    for (int r = 0; r < *rows; r++) {
        char *line = NULL;
        size_t len = 0;
        ssize_t read = getline(&line, &len, fp);
        if (read <= 0 || line[read - 1] != '\n') { free(line); freeMap(map, r); return NULL; }
        line[--read] = '\0';
        if (width == 0) width = read;
        else if (read != width) { free(line); freeMap(map, r); return NULL; }
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
    for (int i = 0; i < rows; i++) dp[i] = calloc(cols, sizeof(int));
    int maxSize = 0, maxRow = 0, maxCol = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (map[i][j] == empty) {
                if (i == 0 || j == 0) dp[i][j] = 1;
                else dp[i][j] = 1 + min3(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]);
                if (dp[i][j] > maxSize) { maxSize = dp[i][j]; maxRow = i; maxCol = j; }
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

void procesar(FILE *fp) {
    int rows = 0, cols = 0;
    char empty = 0, obs = 0, full = 0;
    char **map = leer(fp, &rows, &cols, &empty, &obs, &full);
    if (!map) { fprintf(stderr, "map error\n"); return; }
    resolver(map, rows, cols, empty, obs, full);
    freeMap(map, rows);
}

int main(int argc, char **argv) {
    if (argc == 1) procesar(stdin);
    else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) { fprintf(stderr, "map error\n"); continue; }
            procesar(fp);
            fclose(fp);
        }
    }
    return 0;
}
