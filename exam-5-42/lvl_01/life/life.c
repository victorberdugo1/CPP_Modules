#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int count_neighbors(int h, int w, int *b, int y, int x) {
    int c = 0;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            if (!dy && !dx) continue;
            int ny = y + dy, nx = x + dx;
            if (ny >= 0 && ny < h && nx >= 0 && nx < w) c += b[ny * w + nx];
        }
    return c;
}

void simulate_generation(int h, int w, int *b) {
    int *next = calloc(h * w, sizeof(int));
    if (!next) return;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int n = count_neighbors(h, w, b, y, x);
            int cur = b[y * w + x];
            next[y * w + x] = (cur && (n == 2 || n == 3)) || (!cur && n == 3);
        }
    for (int i = 0; i < h * w; i++) b[i] = next[i];
    free(next);
}

void draw_board(int h, int w, int *b) {
    int x = 0, y = 0, pen = 0;
    char c;
    while (read(0, &c, 1) == 1) {
        if (c == 'w' && y > 0) y--;
        else if (c == 's' && y < h-1) y++;
        else if (c == 'a' && x > 0) x--;
        else if (c == 'd' && x < w-1) x++;
        else if (c == 'x') pen = !pen;
        if (pen) b[y * w + x] = 1;
    }
}

void print_board(int h, int w, int *b) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) putchar(b[y * w + x] ? '0' : ' ');
        putchar('\n');
    }
}

int main(int argc, char **argv) {
    if (argc != 4) return 1;
    int w = atoi(argv[1]), h = atoi(argv[2]), iter = atoi(argv[3]);
    if (w <= 0 || h <= 0 || iter < 0) return 1;
    int *board = calloc(h * w, sizeof(int));
    if (!board) return 1;
    draw_board(h, w, board);
    for (int i = 0; i < iter; i++) simulate_generation(h, w, board);
    print_board(h, w, board);
    free(board);
    return 0;
}

