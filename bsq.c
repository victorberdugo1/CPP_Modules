#include <stdio.h>
#include <stdlib.h>

int min(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

int main(int argc, char **argv) {
    FILE *f = argc > 1 ? fopen(argv[1], "r") : stdin;
    if (!f) return fprintf(stderr, "map error\n"), 1;
    
    char *line = NULL;
    size_t len = 0;
    ssize_t rlen;
    
    // Leer cabecera
    rlen = getline(&line, &len, f);
    if (rlen <= 0) { free(line); return fprintf(stderr, "map error\n"), 1; }
    
    int rows = 0;
    char emp = 0, obs = 0, ful = 0;
    
    // Parsear cabecera
    int i = 0;
    while (i < (int)rlen && line[i] >= '0' && line[i] <= '9')
        rows = rows * 10 + (line[i++] - '0');

    int end = (int)rlen - 1;
    if (end >= 0 && line[end] == '\n') end--;
    
    for (int j = 2; j >= 0; j--) {
        while (end >= 0 && (line[end] == ' ' || line[end] == '\t')) end--;
        if (end < 0) { free(line); return fprintf(stderr, "map error\n"), 1; }
        if (j == 2) ful = line[end--];
        else if (j == 1) obs = line[end--];
        else emp = line[end--];
    }
    
    if (rows <= 0 || emp == obs || emp == ful || obs == ful)
        { free(line); return fprintf(stderr, "map error\n"), 1; }
    
    // Leer mapa
    char **map = malloc(rows * sizeof(char*));
    int cols = -1;
    
    for (int r = 0; r < rows; r++) {
        ssize_t n = getline(&line, &len, f);
        if (n <= 0 || line[n-1] != '\n') {
            while (r-- > 0) free(map[r]);
            free(map); free(line);
            if (f != stdin) fclose(f);
            return fprintf(stderr, "map error\n"), 1;
        }
        
        int clen = n - 1;
        if (cols == -1) cols = clen;
        if (clen != cols || clen <= 0) {
            while (r-- > 0) free(map[r]);
            free(map); free(line);
            if (f != stdin) fclose(f);
            return fprintf(stderr, "map error\n"), 1;
        }
        
        map[r] = malloc(cols + 1);
        for (int j = 0; j < cols; j++) {
            if (line[j] != emp && line[j] != obs) {
                while (r >= 0) free(map[r--]);
                free(map); free(line);
                if (f != stdin) fclose(f);
                return fprintf(stderr, "map error\n"), 1;
            }
            map[r][j] = line[j];
        }
        map[r][cols] = 0;
    }
    free(line);

    // Algoritmo DP
    int *prev = calloc(cols, sizeof(int));
    int *curr = calloc(cols, sizeof(int));
    int max_size = 0, max_r = 0, max_c = 0;
    
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (map[r][c] == obs) curr[c] = 0;
            else {
                curr[c] = (r == 0 || c == 0) ? 1 : 1 + min(prev[c], curr[c-1], prev[c-1]);
                if (curr[c] > max_size) max_size = curr[c], max_r = r, max_c = c;
            }
        }
        int *tmp = prev; prev = curr; curr = tmp;
    }
    // Marcar cuadrado
    if (max_size > 0) {
        for (int r = max_r - max_size + 1; r <= max_r; r++)
            for (int c = max_c - max_size + 1; c <= max_c; c++)
                map[r][c] = ful;
    }
    // Imprimir y limpiar (solo 3 variables ahora)
    for (int r = 0; r < rows; r++) {
        printf("%s\n", map[r]);
        free(map[r]);
    }
    
    free(map);
    free(prev);
    free(curr);   
    if (f != stdin) fclose(f);
    return 0;
}
