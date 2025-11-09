#include <stdio.h>
#include <stdlib.h>

int min(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

char **leer(FILE *f, int *rows, int *cols, char *emp, char *obs, char *ful) {
    char *line = NULL;
    size_t len = 0;
    
    ssize_t rlen = getline(&line, &len, f);
    if (rlen <= 0) return free(line), NULL;
    
    int i = 0;
    *rows = 0;
    while (i < (int)rlen && line[i] >= '0' && line[i] <= '9')
        *rows = *rows * 10 + (line[i++] - '0');
    
    int end = (int)rlen - 1;
    if (end >= 0 && line[end] == '\n') end--;
    
    for (int j = 2; j >= 0; j--) {
        while (end >= 0 && (line[end] == ' ' || line[end] == '\t')) end--;
        if (end < 0) return free(line), NULL;
        if (j == 2) *ful = line[end--];
        else if (j == 1) *obs = line[end--];
        else *emp = line[end--];
    }
    
    if (*rows <= 0 || *emp == *obs || *emp == *ful || *obs == *ful)
        return free(line), NULL;
    
    char **map = malloc(*rows * sizeof(char*));
    *cols = -1;
    
    for (int r = 0; r < *rows; r++) {
        ssize_t n = getline(&line, &len, f);
        if (n <= 0 || line[n-1] != '\n') {
            while (r-- > 0) free(map[r]);
            return free(map), free(line), NULL;
        }
        
        int clen = n - 1;
        if (*cols == -1) *cols = clen;
        if (clen != *cols || clen <= 0) {
            while (r-- > 0) free(map[r]);
            return free(map), free(line), NULL;
        }
        
        map[r] = malloc(*cols + 1);
        for (int j = 0; j < *cols; j++) {
            if (line[j] != *emp && line[j] != *obs) {
                while (r >= 0) free(map[r--]);
                return free(map), free(line), NULL;
            }
            map[r][j] = line[j];
        }
        map[r][*cols] = 0;
    }
    
    return free(line), map;
}

void resolver(int rows, int cols, char **map, char obs, char ful) {
    int *prev = calloc(cols, sizeof(int));
    int *curr = calloc(cols, sizeof(int));
    int sz = 0, sr = 0, sc = 0;
    
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            curr[c] = map[r][c] == obs ? 0 : (c == 0 ? 1 : 1 + min(prev[c], curr[c-1], prev[c-1]));
            if (curr[c] > sz) sz = curr[c], sr = r, sc = c;
        }
        int *t = prev; prev = curr; curr = t;
    }
    
    if (sz > 0)
        for (int r = sr - sz + 1; r <= sr; r++)
            for (int c = sc - sz + 1; c <= sc; c++)
                map[r][c] = ful;
    
    free(prev);
    free(curr);
}

int main(int argc, char **argv) {
    FILE *f = argc > 1 ? fopen(argv[1], "r") : stdin;
    if (!f) return fprintf(stderr, "map error\n"), 1;
    
    int rows, cols;
    char emp, obs, ful;
    char **map = leer(f, &rows, &cols, &emp, &obs, &ful);
    
    if (f != stdin) fclose(f);
    if (!map) return fprintf(stderr, "map error\n"), 1;
    
    resolver(rows, cols, map, obs, ful);
    
    for (int r = 0; r < rows; r++) {
        printf("%s\n", map[r]);
        free(map[r]);
    }
    
    free(map);
    return 0;
}