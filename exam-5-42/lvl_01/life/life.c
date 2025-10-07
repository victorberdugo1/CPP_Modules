#define _POSIX_C_SOURCE 200809L
#include "life.h"
#include <stdio.h>  /* para putchar */
#include <stdlib.h> /* atoi, malloc, calloc, free */

/* Inicializa juego y reserva board; devuelve 0 ok, -1 error. */
int init_game(t_game* game, char* argv[])
{
	game->width = atoi(argv[1]);
	game->height = atoi(argv[2]);
	game->iterations = atoi(argv[3]);

	/* validación mínima de argumentos */
	if (game->width <= 0 || game->height <= 0 || game->iterations < 0)
		return -1;

	game->alive = 'O';
	game->dead  = ' ';
	game->i = 0;
	game->j = 0;
	game->draw = 0;

	/* reservar punteros a filas, calloc para que sean NULL inicialmente */
	game->board = (char **)calloc((size_t)game->height, sizeof(char *));
	if (!game->board) return -1;

	/* reservar cada fila y llenarla con dead */
	for (int r = 0; r < game->height; ++r) {
		game->board[r] = (char *)malloc((size_t)game->width + 1);
		if (!game->board[r]) {
			/* liberar lo ya asignado */
			for (int k = 0; k < r; ++k) free(game->board[k]);
			free(game->board);
			game->board = NULL;
			return -1;
		}
		for (int c = 0; c < game->width; ++c) game->board[r][c] = game->dead;
		game->board[r][game->width] = '\0';
	}
	return 0;
}

/* Lee comandos desde stdin y dibuja usando el "pen". */
void fill_board(t_game* game)
{
	char buf;
	ssize_t rd;
	while ((rd = read(STDIN_FILENO, &buf, 1)) == 1) {
		int is_cmd = 1;
		switch (buf) {
			case 'w':
				if (game->i > 0) game->i--;
				break;
			case 's':
				if (game->i < (game->height - 1)) game->i++;
				break;
			case 'a':
				if (game->j > 0) game->j--;
				break;
			case 'd':
				if (game->j < (game->width - 1)) game->j++;
				break;
			case 'x':
				game->draw = !game->draw;
				break;
			default:
				is_cmd = 0;
				break;
		}
		if (is_cmd && game->draw) {
			/* marcar la celda actual como viva */
			game->board[game->i][game->j] = game->alive;
		}
	}
	(void)rd;
}

/* Cuenta vecinos vivos de la celda (i,j). */
int count_neighbors(t_game* game, int i, int j)
{
	int cnt = 0;
	for (int di = -1; di <= 1; ++di) {
		for (int dj = -1; dj <= 1; ++dj) {
			if (di == 0 && dj == 0) continue;
			int ni = i + di;
			int nj = j + dj;
			if (ni >= 0 && ni < game->height && nj >= 0 && nj < game->width) {
				if (game->board[ni][nj] == game->alive) cnt++;
			}
		}
	}
	return cnt;
}

/* Libera el tablero (usando height) y deja board = NULL. */
void free_board(t_game* game)
{
	if (!game || !game->board) return;
	for (int r = 0; r < game->height; ++r) {
		if (game->board[r]) free(game->board[r]);
	}
	free(game->board);
	game->board = NULL;
}

/* Una iteración de Game of Life; devuelve 0 ok, -1 error (memoria). */
int play(t_game* game)
{
	/* reservar punteros de filas (calloc para NULLs) */
	char **temp = (char **)calloc((size_t)game->height, sizeof(char *));
	if (!temp) return -1;

	/* reservar filas y preparar espacio; en fallo liberar lo asignado */
	for (int r = 0; r < game->height; ++r) {
		temp[r] = (char *)malloc((size_t)game->width + 1);
		if (!temp[r]) {
			for (int k = 0; k < r; ++k) free(temp[k]);
			free(temp);
			return -1;
		}
	}

	/* llenar la nueva generación */
	for (int i = 0; i < game->height; ++i) {
		for (int j = 0; j < game->width; ++j) {
			int n = count_neighbors(game, i, j);
			if (game->board[i][j] == game->alive) {
				if (n == 2 || n == 3) temp[i][j] = game->alive;
				else temp[i][j] = game->dead;
			} else {
				if (n == 3) temp[i][j] = game->alive;
				else temp[i][j] = game->dead;
			}
		}
		temp[i][game->width] = '\0';
	}

	/* reemplazar tablero */
	free_board(game);
	game->board = temp;
	return 0;
}

/* Imprime el tablero usando putchar (cada fila + '\\n'). */
void print_board(t_game* game)
{
	for (int i = 0; i < game->height; ++i) {
		for (int j = 0; j < game->width; ++j) putchar(game->board[i][j]);
		putchar('\n');
	}
}

/* main: validación mínima de args y bucle */
int main(int argc, char* argv[])
{
	if (argc != 4) return 1;

	t_game game;
	if (init_game(&game, argv) == -1) return 1;

	fill_board(&game);

	for (int it = 0; it < game.iterations; ++it) {
		if (play(&game) == -1) {
			free_board(&game);
			return 1;
		}
	}

	print_board(&game);
	free_board(&game);
	return 0;
}

