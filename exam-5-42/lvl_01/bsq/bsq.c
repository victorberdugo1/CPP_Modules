/* bsq.c */
#include "bsq.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/* parsea la cabecera esperando formato con espacios: "N e o f" */
int loadElements(FILE *file, t_elements *elements)
{
	int ret = fscanf(file, "%d %c %c %c",
	                 &(elements->n_lines),
	                 &(elements->empty),
	                 &(elements->obstacle),
	                 &(elements->full));
	if (ret != 4) return -1;
	if (elements->n_lines <= 0) return -1;
	if (elements->empty == elements->obstacle ||
	    elements->empty == elements->full ||
	    elements->obstacle == elements->full) return -1;
	if ((unsigned char)elements->empty < 32 || (unsigned char)elements->empty > 126) return -1;
	if ((unsigned char)elements->obstacle < 32 || (unsigned char)elements->obstacle > 126) return -1;
	if ((unsigned char)elements->full < 32 || (unsigned char)elements->full > 126) return -1;
	return 0;
}

/* libera mapa sabiendo height */
void free_map(t_map *map)
{
	if (!map || !map->grid) return;
	for (int i = 0; i < map->height; ++i) {
		if (map->grid[i]) free(map->grid[i]);
	}
	free(map->grid);
	map->grid = NULL;
}

/* valida que solo aparezcan empty u obstacle en todo el grid */
int element_control(char **grid, int height, char c1, char c2)
{
	for (int i = 0; i < height; ++i) {
		char *row = grid[i];
		for (int j = 0; row[j] != '\0'; ++j) {
			if (row[j] != c1 && row[j] != c2) return -1;
		}
	}
	return 0;
}

/* carga las filas del mapa; exige que cada línea termine en '\n' */
int loadMap(FILE *file, t_map *map, t_elements *elements)
{
	map->height = elements->n_lines;
	/* calloc para inicializar a NULL y evitar UB en free en caso de error */
	map->grid = (char **)calloc((size_t)map->height + 1u, sizeof(char *));
	if (!map->grid) return -1;

	char *line = NULL;
	size_t cap = 0;
	ssize_t read;

	/* consumir el resto de la línea de cabecera (fscanf dejó el '\n') */
	read = getline(&line, &cap, file);
	if (read == -1) { free(line); free_map(map); return -1; }

	for (int i = 0; i < map->height; ++i) {
		read = getline(&line, &cap, file);
		if (read == -1) { free(line); free_map(map); return -1; }

		/* cada línea debe terminar en '\n' según el enunciado */
		if (read == 0 || line[read - 1] != '\n') { free(line); free_map(map); return -1; }

		/* quitar '\n' */
		line[read - 1] = '\0';
		--read;

		if (read <= 0) { free(line); free_map(map); return -1; }

		if (i == 0) {
			map->width = (int)read;
			if (map->width <= 0) { free(line); free_map(map); return -1; }
		} else {
			if ((int)read != map->width) { free(line); free_map(map); return -1; }
		}

		/* asignar y copiar */
		map->grid[i] = (char *)malloc((size_t)map->width + 1);
		if (!map->grid[i]) { free(line); free_map(map); return -1; }
		for (int j = 0; j < map->width; ++j) map->grid[i][j] = line[j];
		map->grid[i][map->width] = '\0';
	}

	/* validar caracteres */
	if (element_control(map->grid, map->height, elements->empty, elements->obstacle) == -1) {
		free(line);
		free_map(map);
		return -1;
	}

	free(line);
	return 0;
}

/* mínimo de 3 enteros */
static int find_min(int a, int b, int c)
{
	int m = a;
	if (b < m) m = b;
	if (c < m) m = c;
	return m;
}

/* DP con matriz dinámica */
void find_big_square(t_map *map, t_square *square, t_elements *elements)
{
	int h = map->height;
	int w = map->width;
	int *mat = (int *)calloc((size_t)h * (size_t)w, sizeof(int));
	if (!mat) return;

	#define AT(i,j) mat[(i) * w + (j)]

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			if (map->grid[i][j] == elements->obstacle) {
				AT(i,j) = 0;
			} else if (i == 0 || j == 0) {
				AT(i,j) = 1;
			} else {
				int m = find_min(AT(i-1,j), AT(i-1,j-1), AT(i,j-1));
				AT(i,j) = m + 1;
			}
			if (AT(i,j) > square->size) {
				square->size = AT(i,j);
				square->i = i - AT(i,j) + 1;
				square->j = j - AT(i,j) + 1;
			}
		}
	}

	free(mat);
	#undef AT
}

/* pinta y muestra el resultado */
void print_filled_square(t_map *map, t_square *square, t_elements *elements)
{
	for (int i = square->i; i < square->i + square->size; ++i) {
		for (int j = square->j; j < square->j + square->size; ++j) {
			if (i >= 0 && i < map->height && j >= 0 && j < map->width)
				map->grid[i][j] = elements->full;
		}
	}
	for (int i = 0; i < map->height; ++i) {
		fputs(map->grid[i], stdout);
		fputc('\n', stdout);
	}
}

/* ejecuta bsq sobre un FILE* */
int execute_bsq(FILE *file)
{
	t_elements elements;
	if (loadElements(file, &elements) == -1) return -1;

	t_map map;
	map.grid = NULL; map.width = 0; map.height = 0;
	if (loadMap(file, &map, &elements) == -1) return -1;

	t_square square;
	square.size = 0; square.i = 0; square.j = 0;
	find_big_square(&map, &square, &elements);
	print_filled_square(&map, &square, &elements);
	free_map(&map);
	return 0;
}

/* abrir archivo y ejecutar */
int convert_file_pointer(char *name)
{
	FILE *file = fopen(name, "r");
	if (!file) return -1;
	int ret = execute_bsq(file);
	fclose(file);
	return ret;
}

int main(int argc, char **argv)
{
	if (argc == 1) {
		if (execute_bsq(stdin) == -1)
			fprintf(stderr, "map error\n");
	} else {
		for (int i = 1; i < argc; ++i) {
			if (convert_file_pointer(argv[i]) == -1)
				fprintf(stderr, "map error\n");
			if (i < argc - 1) fputc('\n', stdout);
		}
	}
	return 0;
}

