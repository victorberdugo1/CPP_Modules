#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int min3(int a, int b, int c) { return a < b ? (a < c ? a : c) : (b < c ? b : c); }

int process_stream(FILE *f) {
    int rows;
    char empty, obs, full;    
    // Leer cabecera
    if (fscanf(f, "%d %c %c %c\n", &rows, &empty, &obs, &full) != 4) return 0;
    if (rows <= 0 || empty == obs || empty == full || obs == full) return 0;
    // Leer mapa
    char **map = calloc(rows, sizeof(char*));
    if (!map) return 0;

    int cols = -1;
    int valid = 1;
    
    for (int i = 0; i < rows && valid; i++) {
        size_t len = 0;
        if (getline(&map[i], &len, f) == -1) {
            valid = 0;
            break;
        }        
        int line_len = strlen(map[i]);
        if (line_len > 0 && map[i][line_len-1] == '\n') {
            map[i][--line_len] = '\0';
        }
        
        if (cols == -1) cols = line_len;
        else if (line_len != cols) valid = 0;
        // Validar caracteres
        for (int j = 0; j < cols && valid; j++) {
            if (map[i][j] != empty && map[i][j] != obs) valid = 0;
        }
    }
    if (!valid || cols <= 0) {
        for (int i = 0; i < rows; i++) {
            if (map[i]) free(map[i]);
        }
        free(map);
        return 0;
    }
    // Calcular DP
    int total = rows * cols;
    int *dp = calloc(total, sizeof(int));
    if (!dp) {
        for (int i = 0; i < rows; i++) free(map[i]);
        free(map);
        return 0;
    }
    int max_size = 0, max_i = 0, max_j = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            if (map[i][j] == obs) {
                dp[idx] = 0;
            } else {
                if (i == 0 || j == 0) {
                    dp[idx] = 1;
                } else {
                    dp[idx] = min3(dp[(i-1)*cols + j], 
                                  dp[i*cols + (j-1)], 
                                  dp[(i-1)*cols + (j-1)]) + 1;
                }
                if (dp[idx] > max_size) {
                    max_size = dp[idx];
                    max_i = i;
                    max_j = j;
                }
            }
        }
    }
    // Marcar cuadrado
    if (max_size > 0) {
        for (int i = max_i - max_size + 1; i <= max_i; i++) {
            for (int j = max_j - max_size + 1; j <= max_j; j++) {
                map[i][j] = full;
            }
        }
    }
    // Imprimir y liberar memoria
    for (int i = 0; i < rows; i++) {
        printf("%s\n", map[i]);
        free(map[i]);
    }
    free(map);
    free(dp);
    return 1;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        if (!process_stream(stdin)) fprintf(stderr, "map error\n");
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *f = fopen(argv[i], "r");
            if (!f) {
                fprintf(stderr, "map error\n");
            } else {
                if (!process_stream(f)) fprintf(stderr, "map error\n");
                fclose(f);
            }
            if (i < argc - 1) printf("\n");
        }
    }
    return 0;
}
