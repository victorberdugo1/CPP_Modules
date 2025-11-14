#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int contar(int filas, int cols, int *tab, int fila, int col){
	int vec = 0;
	for (int dfil = -1; dfil <= 1; dfil ++){
		for (int dcol = -1; dcol <= 1; dcol ++){
			if (dfil == 0 && dcol == 0) continue;
			int f2 = fila + dfil;
			int c2 = col + dcol;
			if (f2 >= 0 && f2 < filas && c2 >= 0 && c2 < cols)
				vec += tab[f2 * cols + c2];
		}
	}
	return vec;
}

void sig(int filas, int cols, int *tab){
	int *nuevo = calloc(filas * cols, sizeof(int));
	if (!nuevo) return;
	for(int fila = 0; fila < filas; fila++){
		for(int col = 0; col < cols; col++){
			int vec = contar(filas, cols, tab, fila,col);
			int cel = tab[fila * cols + col];
			nuevo[fila * cols + col] = (cel && (vec == 2 || vec == 3)) || (!cel && vec ==3);
		}

	}
	for (int i = 0; i < filas * cols; i ++)
		tab[i] = nuevo [i];
	free(nuevo);
}

void mostrar(int filas, int cols, int *tab)
{
	for(int fila = 0; fila < filas; fila++){
		for(int col = 0; col < cols; col++){
			putchar(tab[fila * cols + col] ? 'O' : ' ');
		}
		putchar ('\n');
	}
}

int main(int c, char **v){
	if ( c != 4) return 1;
	int cols = atoi(v[1]), filas = atoi(v[2]), gens = atoi(v[3]);
	if (cols <= 0 || filas <= 0 || gens < 0) return 1;
	int* tab = calloc(filas * cols, sizeof(int));
	if(!tab) return 1;

	int col = 0, fila = 0, lapiz = 0;
	char t;
	while(read(0,&t,1) == 1){
		if(t == 'w' && fila >= 0) fila--;
		else if(t == 's' && fila < filas -1) fila++;
		else if(t == 'a' && col >= 0) col--;
		else if(t == 'd' && col < cols - 1) col ++;
		else if(t == 'x') lapiz = !lapiz;
		if (lapiz) tab[fila * cols + col] = 1;
	}

	for(int g = 0; g < gens; g++){
		sig(filas, cols, tab);
	}

	mostrar(filas, cols, tab);
	free(tab);
	return 0;
}
