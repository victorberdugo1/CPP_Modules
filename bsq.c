#include <stdio.h>
#include <stdlib.h>

int min(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

char **leer(FILE *file, int *nrows, int *ncols, char *empty, char *obst, char *fill) {
    char *buf = NULL;
    size_t cap = 0;
    
    ssize_t len = getline(&buf, &cap, file);
    if (len <= 0) return free(buf), NULL;
    
    int idx = 0;
    *nrows = 0;
    while (idx < (int)len && buf[idx] >= '0' && buf[idx] <= '9')
        *nrows = *nrows * 10 + (buf[idx++] - '0');
    
    int last = (int)len - 1;
    if (last >= 0 && buf[last] == '\n') last--;
    
    for (int i = 2; i >= 0; i--) {
        while (last >= 0 && (buf[last] == ' ' || buf[last] == '\t')) last--;
        if (last < 0) return free(buf), NULL;
        if (i == 2) *fill = buf[last--];
        else if (i == 1) *obst = buf[last--];
        else *empty = buf[last--];
    }
    
    if (*nrows <= 0 || *empty == *obst || *empty == *fill || *obst == *fill)
        return free(buf), NULL;
    
    char **grid = malloc(*nrows * sizeof(char*));
    *ncols = -1;
    
    for (int row = 0; row < *nrows; row++) {
        ssize_t rlen = getline(&buf, &cap, file);
        if (rlen <= 0 || buf[rlen-1] != '\n') {
            while (row-- > 0) free(grid[row]);
            return free(grid), free(buf), NULL;
        }
        
        int width = rlen - 1;
        if (*ncols == -1) *ncols = width;
        if (width != *ncols || width <= 0) {
            while (row-- > 0) free(grid[row]);
            return free(grid), free(buf), NULL;
        }
        
        grid[row] = malloc(*ncols + 1);
        for (int col = 0; col < *ncols; col++) {
            if (buf[col] != *empty && buf[col] != *obst) {
                while (row >= 0) free(grid[row--]);
                return free(grid), free(buf), NULL;
            }
            grid[row][col] = buf[col];
        }
        grid[row][*ncols] = 0;
    }
    
    return free(buf), grid;
}

void resolver(int nrows, int ncols, char **grid, char obst, char fill) {
    int *dpPrev = calloc(ncols, sizeof(int));
    int *dpCurr = calloc(ncols, sizeof(int));
    int maxSz = 0, maxRow = 0, maxCol = 0;
    
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            dpCurr[col] = grid[row][col] == obst ? 0 
                : (col == 0 ? 1 : 1 + min(dpPrev[col], dpCurr[col-1], dpPrev[col-1]));
            
            if (dpCurr[col] > maxSz) {
                maxSz = dpCurr[col];
                maxRow = row;
                maxCol = col;
            }
        }
        int *tmp = dpPrev;
        dpPrev = dpCurr;
        dpCurr = tmp;
    }
    
    if (maxSz > 0) {
        for (int row = maxRow - maxSz + 1; row <= maxRow; row++)
            for (int col = maxCol - maxSz + 1; col <= maxCol; col++)
                grid[row][col] = fill;
    }
    
    free(dpPrev);
    free(dpCurr);
}

int main(int argc, char **argv) {
    FILE *file = argc > 1 ? fopen(argv[1], "r") : stdin;
    if (!file) return fprintf(stderr, "map error\n"), 1;
    
    int nrows, ncols;
    char empty, obst, fill;
    char **grid = leer(file, &nrows, &ncols, &empty, &obst, &fill);
    
    if (file != stdin) fclose(file);
    if (!grid) return fprintf(stderr, "map error\n"), 1;
    
    resolver(nrows, ncols, grid, obst, fill);
    
    for (int row = 0; row < nrows; row++) {
        printf("%s\n", grid[row]);
        free(grid[row]);
    }
    
    free(grid);
    return 0;
}
