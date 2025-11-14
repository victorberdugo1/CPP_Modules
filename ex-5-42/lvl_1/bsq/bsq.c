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
    if (len <= 0) {
        free(buf);
        return NULL;
    }
    
    // Parse number of rows
    int idx = 0;
    *nrows = 0;
    while (idx < (int)len && buf[idx] >= '0' && buf[idx] <= '9')
        *nrows = *nrows * 10 + (buf[idx++] - '0');
    
    // Find last non-newline character
    int last = (int)len - 1;
    if (last >= 0 && buf[last] == '\n') last--;
    
    // Parse characters from right to left: fill, obst, empty
    for (int i = 2; i >= 0; i--) {
        while (last >= 0 && (buf[last] == ' ' || buf[last] == '\t')) last--;
        if (last < 0) {
            free(buf);
            return NULL;
        }
        if (i == 2) *fill = buf[last--];
        else if (i == 1) *obst = buf[last--];
        else *empty = buf[last--];
    }
    
    // Validate parameters
    if (*nrows <= 0 || *empty == *obst || *empty == *fill || *obst == *fill) {
        free(buf);
        return NULL;
    }
    
    // Allocate grid
    char **grid = malloc(*nrows * sizeof(char*));
    if (!grid) {
        free(buf);
        return NULL;
    }
    
    *ncols = -1;
    
    // Read grid lines
    for (int row = 0; row < *nrows; row++) {
        ssize_t rlen = getline(&buf, &cap, file);
        if (rlen <= 0 || buf[rlen-1] != '\n') {
            while (row-- > 0) free(grid[row]);
            free(grid);
            free(buf);
            return NULL;
        }
        
        int width = rlen - 1;
        if (*ncols == -1) *ncols = width;
        if (width != *ncols || width <= 0) {
            while (row-- > 0) free(grid[row]);
            free(grid);
            free(buf);
            return NULL;
        }
        
        grid[row] = malloc(*ncols + 1);
        if (!grid[row]) {
            while (row-- > 0) free(grid[row]);
            free(grid);
            free(buf);
            return NULL;
        }
        
        for (int col = 0; col < *ncols; col++) {
            if (buf[col] != *empty && buf[col] != *obst) {
                while (row >= 0) {
                    free(grid[row]);
                    row--;
                }
                free(grid);
                free(buf);
                return NULL;
            }
            grid[row][col] = buf[col];
        }
        grid[row][*ncols] = '\0';
    }
    
    free(buf);
    return grid;
}

void resolver(int nrows, int ncols, char **grid, char obst, char fill) {
    int *dpPrev = calloc(ncols, sizeof(int));
    int *dpCurr = calloc(ncols, sizeof(int));
    
    if (!dpPrev || !dpCurr) {
        free(dpPrev);
        free(dpCurr);
        return;
    }
    
    int maxSz = 0, maxRow = 0, maxCol = 0;
    
    // Dynamic programming to find largest square
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            if (grid[row][col] == obst) {
                dpCurr[col] = 0;
            } else if (row == 0 || col == 0) {
                dpCurr[col] = 1;
            } else {
                dpCurr[col] = 1 + min(dpPrev[col], dpCurr[col-1], dpPrev[col-1]);
            }
            
            if (dpCurr[col] > maxSz) {
                maxSz = dpCurr[col];
                maxRow = row;
                maxCol = col;
            }
        }
        // Swap buffers
        int *tmp = dpPrev;
        dpPrev = dpCurr;
        dpCurr = tmp;
    }
    
    // Fill the square
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
    if (!file) {
        fprintf(stderr, "map error\n");
        return 1;
    }
    
    int nrows, ncols;
    char empty, obst, fill;
    char **grid = leer(file, &nrows, &ncols, &empty, &obst, &fill);
    
    if (file != stdin) fclose(file);
    
    if (!grid) {
        fprintf(stderr, "map error\n");
        return 1;
    }
    
    resolver(nrows, ncols, grid, obst, fill);
    
    for (int row = 0; row < nrows; row++) {
        printf("%s\n", grid[row]);
        free(grid[row]);
    }
    
    free(grid);
    return 0;
}
