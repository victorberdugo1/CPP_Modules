/* bsq.h */
#ifndef BSQ_H
#define BSQ_H

#include <stdio.h>
#include <stdlib.h>

typedef struct s_elements {
	int n_lines;
	char empty;
	char obstacle;
	char full;
} t_elements;

typedef struct s_map {
	char **grid;
	int width;
	int height;
} t_map;

typedef struct s_square {
	int size;
	int i;
	int j;
} t_square;

/* parsea la cabecera esperando formato con espacios: "N e o f" */
int loadElements(FILE *file, t_elements *elements);

/* libera mapa sabiendo height */
void free_map(t_map *map);

/* valida que solo aparezcan empty u obstacle en todo el grid */
int element_control(char **grid, int height, char c1, char c2);

/* carga las filas del mapa; exige que cada línea termine en '\n' */
int loadMap(FILE *file, t_map *map, t_elements *elements);

/* DP con matriz dinámica para encontrar el mayor cuadrado */
void find_big_square(t_map *map, t_square *square, t_elements *elements);

/* pinta y muestra el resultado */
void print_filled_square(t_map *map, t_square *square, t_elements *elements);

/* ejecuta bsq sobre un FILE* */
int execute_bsq(FILE *file);

/* abrir archivo y ejecutar */
int convert_file_pointer(char *name);

#endif /* BSQ_H */

