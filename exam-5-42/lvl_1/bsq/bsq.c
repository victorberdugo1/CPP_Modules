#include <stdio.h>
#include <stdlib.h>

int str_len(char *text) {
    int i = 0;
    while (text[i]) i++;
    return i;
}

void free_map(char **map, int rows) {
    if (!map) return;
    for (int i = 0; i < rows; i++)
        free(map[i]);
    free(map);
}

int read_map(FILE *fp, char ***map, int *rows, int *cols, char *empty, char *obstacle, char *full) {
    char *line = NULL;
    size_t capacity = 0;
    ssize_t read_len;

    if (fscanf(fp, "%d %c %c %c\n", rows, empty, obstacle, full) != 4)
        return 0;
    if (*rows <= 0 || *empty == *obstacle || *empty == *full || *obstacle == *full)
        return 0;

    *map = malloc(sizeof(char*) * (*rows));
    if (!*map)
        return 0;

    *cols = 0;
    for (int i = 0; i < *rows; i++) {
        read_len = getline(&line, &capacity, fp);
        if (read_len <= 0) {
            free(line);
            free_map(*map, i);
            return 0;
        }
        if (line[read_len - 1] == '\n')
            line[--read_len] = 0;

        int len = str_len(line);
        if (len == 0) {
            free(line);
            free_map(*map, i);
            return 0;
        }
        if (*cols == 0)
            *cols = len;
        else if (len != *cols) {
            free(line);
            free_map(*map, i);
            return 0;
        }

        for (int j = 0; j < len; j++) {
            if (line[j] != *empty && line[j] != *obstacle) {
                free(line);
                free_map(*map, i);
                return 0;
            }
        }

        (*map)[i] = line;
        line = NULL;
        capacity = 0;
    }
    return 1;
}

int min3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

void solve_map(char **map, int rows, int cols, char empty, char obstacle, char full) {
    int *prev = calloc(cols, sizeof(int));
    int *curr = calloc(cols, sizeof(int));
    if (!prev || !curr) {
        free(prev);
        free(curr);
        return;
    }

    int best = 0, best_row = 0, best_col = 0;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (map[i][j] == obstacle)
                curr[j] = 0;
            else {
                if (i == 0 || j == 0)
                    curr[j] = 1;
                else
                    curr[j] = 1 + min3(prev[j], curr[j - 1], prev[j - 1]);

                if (curr[j] > best) {
                    best = curr[j];
                    best_row = i - best + 1;
                    best_col = j - best + 1;
                } else if (curr[j] == best && best > 0) {
                    int new_row = i - curr[j] + 1;
                    int new_col = j - curr[j] + 1;
                    if (new_row < best_row || (new_row == best_row && new_col < best_col)) {
                        best_row = new_row;
                        best_col = new_col;
                    }
                }
            }
        }
        for (int k = 0; k < cols; k++) {
            prev[k] = curr[k];
            curr[k] = 0;
        }
    }

    free(prev);
    free(curr);

    for (int r = best_row; r < best_row + best; r++)
        for (int c = best_col; c < best_col + best; c++)
            map[r][c] = full;

    for (int i = 0; i < rows; i++) {
        fputs(map[i], stdout);
        fputs("\n", stdout);
    }
}

void process(FILE *fp) {
    char **map = NULL;
    int rows = 0, cols = 0;
    char empty = 0, obstacle = 0, full = 0;

    if (!read_map(fp, &map, &rows, &cols, &empty, &obstacle, &full)) {
        fprintf(stderr, "map error\n");
        return;
    }

    solve_map(map, rows, cols, empty, obstacle, full);
    free_map(map, rows);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        process(stdin);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp)
                fprintf(stderr, "map error\n");
            else {
                process(fp);
                fclose(fp);
            }
            if (i < argc - 1)
                fputs("\n", stdout);
        }
    }
    return 0;
}
