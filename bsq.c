#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c){
	return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

int leer_cab(FILE *f, int *rows, char *vac, char *obs, char *full){
	char *line = NULL;
	size_t len = 0;
	ssize_t r = getline(&line, &len, f);
	if (r <= 0) {free(line); return 0;}
	*rows = 0;
	for (int i = 0; line[i] >= '0' && line[i] <= '9'; i++)
		*rows = *rows * 10 + (line[i] - '0');
	if (*rows <= 0 || r < 4) {free(line); return 0;}
	*full = line[r - 2];
	*obs = line[r - 3];
	*vac = line[r - 4];
	int ok = (*vac != *obs && *vac != *full && *obs != *full);
	free(line);
	return ok;
}

char *leer_mapa(FILE *f, int rows, int *cols, char vac, char obs){
	char *line = NULL;
	size_t len = 0;
	if (getline(&line, &len, f) <= 0) {free(line); return NULL;}
	*cols = 0;
	while (line[*cols] && line[*cols] != '\n') (*cols)++;
	char *tab = malloc(rows * (*cols));
	if (!tab) {free(line); return NULL;}
	for (int i = 0; i < rows; i++){
		if (i > 0 && getline(&line, &len, f) <= 0) {free(line); free(tab); return NULL;}
		for (int c = 0; c < *cols; c++){
			if (line[c] != vac && line[c] != obs) {free(line); free(tab); return NULL;}
			tab[i * (*cols) + c] = line[c];
		}
	}
	free(line);
	return tab;
}

void resolver(int rows, int cols, char *tab, char obs, char full){
	int *prev = calloc(cols, sizeof(int));
	int *curr = calloc(cols, sizeof(int));
	if (!prev || !curr) {free(prev); free(curr); return;}
	int best = 0, br = 0, bc = 0;
	for (int r = 0; r < rows; r++){
		for (int c = 0; c < cols; c++){
			if (tab[r * cols + c] == obs)
				curr[c] = 0;
			else if (r == 0 || c == 0)
				curr[c] = 1;
			else
				curr[c] = 1 + min3(prev[c], curr[c - 1], prev[c - 1]);
			if (curr[c] > best){
				best = curr[c];
				br = r;
				bc = c;
			}
		}
		int *tmp = prev; prev = curr; curr = tmp;
		for (int c = 0; c < cols; c++) curr[c] = 0;
	}
	for (int r = br - best + 1; r <= br; r++)
		for (int c = bc - best + 1; c <= bc; c++)
			tab[r * cols + c] = full;
	free(prev);
	free(curr);
}

void mostrar(int rows, int cols, char *tab){
	for (int r = 0; r < rows; r++){
		for (int c = 0; c < cols; c++)
			putchar(tab[r * cols + c]);
		putchar('\n');
	}
}

int main(int argc, char **argv){
	FILE *f = argc > 1 ? fopen(argv[1], "r") : stdin;
	if (!f) return fprintf(stderr, "map error\n"), 1;
	int rows, cols;
	char vac, obs, full;
	if (!leer_cab(f, &rows, &vac, &obs, &full)){
		if (f != stdin) fclose(f);
		return fprintf(stderr, "map error\n"), 1;
	}
	char *tab = leer_mapa(f, rows, &cols, vac, obs);
	if (f != stdin) fclose(f);
	if (!tab) return fprintf(stderr, "map error\n"), 1;
	resolver(rows, cols, tab, obs, full);
	mostrar(rows, cols, tab);
	free(tab);
	return 0;
}