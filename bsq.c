#include <stdio.h>
#include <stdlib.h>

int main(int c, char **v) {
    FILE *fd = (c == 2) ? fopen(v[1], "r") : stdin;
    if (!fd) {
        perror("fopen");
        return 1;
    }

    int lines;
    char emp, obs, full;

    if (fscanf(fd, "%d %c %c %c", &lines, &emp, &obs, &full) == 4)
        printf("Leidos: %d %c %c %c\n", lines, emp, obs, full);
    else
        fprintf(stderr, "Error de lectura o formato\n");

    if (fd != stdin)
        fclose(fd);
    return 0;
}
