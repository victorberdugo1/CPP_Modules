// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int min3(int a, int b, int c) {
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

/* Lee la cabecera y el mapa; devuelve un array de filas (strings).
   Formato esperado primera línea: <rows><c1><c2><c3>\n
   Donde c1 = caracter vacío, c2 = obstáculo, c3 = relleno. */
char **leer_mapa(FILE *f, int *rows, int *cols, char *vac, char *obs, char *relleno) {
	char *line = NULL;
	size_t len = 0;
	ssize_t r = getline(&line, &len, f);
	if (r <= 0) { free(line); return NULL; }

	/* leer número de filas */
	char *p = line;
	long rr = strtol(p, &p, 10);
	if (rr <= 0) { free(line); return NULL; }
	*rows = (int)rr;

	/* buscar los 3 últimos caracteres no-espacio (antes del '\n') */
	int i = (int)r - 1;
	while (i >= 0 && (line[i] == '\n' || line[i] == ' ' || line[i] == '\t')) i--;
	if (i < 2) { free(line); return NULL; }
	*relleno = line[i--];
	*obs     = line[i--];
	*vac     = line[i--];
	if (*vac == *obs || *vac == *relleno || *obs == *relleno) { free(line); return NULL; }

	free(line);

	char **map = malloc((*rows) * sizeof(char*));
	if (!map) return NULL;
	*cols = -1;

	for (int rrow = 0; rrow < *rows; rrow++) {
		ssize_t n = getline(&line, &len, f);
		if (n <= 0 || line[n-1] != '\n') {
			for (int k = 0; k < rrow; k++) free(map[k]);
			free(map); free(line);
			return NULL;
		}
		int clen = (int)n - 1;
		if (*cols == -1) *cols = clen;
		if (clen != *cols || clen <= 0) {
			for (int k = 0; k < rrow; k++) free(map[k]);
			free(map); free(line);
			return NULL;
		}
		map[rrow] = malloc(*cols + 1);
		if (!map[rrow]) {
			for (int k = 0; k < rrow; k++) free(map[k]);
			free(map); free(map); return NULL;
		}
		for (int c = 0; c < *cols; c++) {
			if (line[c] != *vac && line[c] != *obs) {
				for (int k = 0; k <= rrow; k++) free(map[k]);
				free(map); free(line); return NULL;
			}
			map[rrow][c] = line[c];
		}
		map[rrow][*cols] = '\0';
	}
	free(line);
	return map;
}

/* Resuelve el BSQ usando programación dinámica por filas.
   Mantengo dos arrays de ints: prev (fila r-1) y curr (fila r). */
void resolver_bsq(int rows, int cols, char **map, char obs, char relleno) {
	int *prev = calloc(cols, sizeof(int));
	int *curr = calloc(cols, sizeof(int));
	if (!prev || !curr) { free(prev); free(curr); return; }

	int best_size = 0, best_r = 0, best_c = 0;

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			if (map[r][c] == obs) {
				curr[c] = 0;
			} else {
				if (c == 0) curr[c] = 1;
				else curr[c] = 1 + min3(prev[c], curr[c-1], prev[c-1]);
			}
			if (curr[c] > best_size) {
				best_size = curr[c];
				best_r = r;
				best_c = c;
			}
		}
		/* preparar siguiente fila: prev <- curr, y poner curr a cero */
		int *tmp = prev; prev = curr; curr = tmp;
		memset(curr, 0, cols * sizeof(int));
	}

	/* marcar la mejor submatriz encontrada */
	if (best_size > 0) {
		for (int r = best_r - best_size + 1; r <= best_r; r++)
			for (int c = best_c - best_size + 1; c <= best_c; c++)
				map[r][c] = relleno;
	}

	free(prev);
	free(curr);
}

int main(int argc, char **argv) {
	FILE *f = argc > 1 ? fopen(argv[1], "r") : stdin;
	if (!f) return fprintf(stderr, "map error\n"), 1;

	int rows, cols;
	char vac, obs, relleno;
	char **map = leer_mapa(f, &rows, &cols, &vac, &obs, &relleno);
	if (f != stdin) fclose(f);
	if (!map) return fprintf(stderr, "map error\n"), 1;

	resolver_bsq(rows, cols, map, obs, relleno);

	for (int r = 0; r < rows; r++) {
		printf("%s\n", map[r]);
		free(map[r]);
	}
	free(map);
	return 0;
}
