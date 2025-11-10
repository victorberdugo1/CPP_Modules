#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c) {
	return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

/* Lee cabecera del mapa y obtiene filas y caracteres */
int leer_cabecera(FILE *f, int *rows, char *vac, char *obs, char *full) {
	char *line = NULL;
	size_t len = 0;
	ssize_t r = getline(&line, &len, f);
	if (r <= 0) { free(line); return 0; }

	int i = 0;
	*rows = 0;
	while (line[i] >= '0' && line[i] <= '9') {
		*rows = *rows * 10 + (line[i] - '0');
		i++;
	}

	if (*rows <= 0 || r < 4) { free(line); return 0; }

	/* últimos 3 caracteres antes del '\n' */
	int end = (int)r - 2;
	if (end < 2) { free(line); return 0; }

	*full = line[end--];
	*obs  = line[end--];
	*vac  = line[end--];

	if (*vac == *obs || *vac == *full || *obs == *full) {
		free(line);
		return 0;
	}

	free(line);
	return 1;
}

/* Lee el mapa completo */
char **leer_mapa(FILE *f, int rows, int *cols, char vac, char obs) {
	char **map = malloc(rows * sizeof(char *));
	if (!map) return NULL;

	char *line = NULL;
	size_t len = 0;
	*cols = -1;

	for (int i = 0; i < rows; i++) {
		ssize_t r = getline(&line, &len, f);
		if (r <= 0) { free(line); for (int k = 0; k < i; k++) free(map[k]); free(map); return NULL; }

		if (line[r - 1] == '\n') line[r - 1] = '\0';
		int clen = 0;
		while (line[clen]) clen++;

		if (*cols == -1) *cols = clen;
		else if (clen != *cols) {
			free(line); for (int k = 0; k < i; k++) free(map[k]); free(map);
			return NULL;
		}

		map[i] = malloc(clen + 1);
		if (!map[i]) { free(line); for (int k = 0; k < i; k++) free(map[k]); free(map); return NULL; }

		for (int j = 0; j < clen; j++) {
			if (line[j] != vac && line[j] != obs) {
				free(line);
				for (int k = 0; k <= i; k++) free(map[k]);
				free(map);
				return NULL;
			}
			map[i][j] = line[j];
		}
		map[i][clen] = '\0';
	}
	free(line);
	return map;
}

/* Algoritmo BSQ */
void resolver_bsq(int rows, int cols, char **map, char obs, char full) {
	int *prev = calloc(cols, sizeof(int));
	int *curr = calloc(cols, sizeof(int));
	if (!prev || !curr) { free(prev); free(curr); return; }

	int best_size = 0, best_r = 0, best_c = 0;

	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			if (map[r][c] == obs)
				curr[c] = 0;
			else if (r == 0 || c == 0)
				curr[c] = 1;
			else
				curr[c] = 1 + min3(prev[c], curr[c - 1], prev[c - 1]);

			if (curr[c] > best_size) {
				best_size = curr[c];
				best_r = r;
				best_c = c;
			}
		}
		/* swap prev y curr */
		for (int c = 0; c < cols; c++) {
			prev[c] = curr[c];
			curr[c] = 0;
		}
	}

	if (best_size > 0) {
		for (int r = best_r - best_size + 1; r <= best_r; r++)
			for (int c = best_c - best_size + 1; c <= best_c; c++)
				map[r][c] = full;
	}

	free(prev);
	free(curr);
}

int main(int argc, char **argv) {
	FILE *f = argc > 1 ? fopen(argv[1], "r") : stdin;
	if (!f) return fprintf(stderr, "map error\n"), 1;

	int rows, cols;
	char vac, obs, full;
	if (!leer_cabecera(f, &rows, &vac, &obs, &full)) {
		if (f != stdin) fclose(f);
		return fprintf(stderr, "map error\n"), 1;
	}

	char **map = leer_mapa(f, rows, &cols, vac, obs);
	if (f != stdin) fclose(f);
	if (!map) return fprintf(stderr, "map error\n"), 1;

	resolver_bsq(rows, cols, map, obs, full);

	for (int r = 0; r < rows; r++) {
		fputs(map[r], stdout);
		fputc('\n', stdout);
		free(map[r]);
	}
	free(map);
	return 0;
}

