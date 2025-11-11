#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

int main(int argc, char **argv) {
    FILE *f = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (!f) { fprintf(stderr, "map error\n"); return 1; }
    
    // Leer primera línea: "9.ox"
    char *linea = NULL;
    size_t tam = 0;
    ssize_t len = getline(&linea, &tam, f);
    
    // Extraer filas y caracteres
    int filas = atoi(linea);
    char vacio = linea[len-4], obs = linea[len-3], lleno = linea[len-2];
    free(linea);
    
    if (filas <= 0 || vacio == obs || vacio == lleno || obs == lleno) {
        if (f != stdin) fclose(f);
        fprintf(stderr, "map error\n");
        return 1;
    }
    
    // Leer primera fila para saber columnas
    linea = NULL;
    len = getline(&linea, &tam, f);
    int cols = 0;
    while (linea[cols] && linea[cols] != '\n') cols++;
    
    // Crear mapa
    char *mapa = malloc(filas * cols);
    int *t = calloc(filas * cols, sizeof(int));
    if (!mapa || !t) {
        free(linea); free(mapa); free(t);
        if (f != stdin) fclose(f);
        fprintf(stderr, "map error\n");
        return 1;
    }
    
    // Copiar primera fila
    for (int c = 0; c < cols; c++) mapa[c] = linea[c];
    
    // Leer resto de filas
    for (int r = 1; r < filas; r++) {
        getline(&linea, &tam, f);
        for (int c = 0; c < cols; c++) mapa[r * cols + c] = linea[c];
    }
    
    free(linea);
    if (f != stdin) fclose(f);
    
    // ALGORITMO DP (lo importante)
    int max_tam = 0, max_r = 0, max_c = 0;
    
    for (int r = 0; r < filas; r++) {
        for (int c = 0; c < cols; c++) {
            int pos = r * cols + c;
            
            if (mapa[pos] == obs) {
                t[pos] = 0;
            } else if (r == 0 || c == 0) {
                t[pos] = 1;
            } else {
                t[pos] = 1 + min3(t[pos - cols], t[pos - 1], t[pos - cols - 1]);
            }
            
            if (t[pos] > max_tam) {
                max_tam = t[pos];
                max_r = r;
                max_c = c;
            }
        }
    }
    
    free(t);
    
    // Rellenar cuadrado
    for (int r = max_r - max_tam + 1; r <= max_r; r++)
        for (int c = max_c - max_tam + 1; c <= max_c; c++)
            mapa[r * cols + c] = lleno;
    
    // Imprimir
    for (int r = 0; r < filas; r++) {
        for (int c = 0; c < cols; c++) putchar(mapa[r * cols + c]);
        putchar('\n');
    }
    
    free(mapa);
    return 0;
}

/*
#include <stdio.h>
#include <stdlib.h>

int min3(int a, int b, int c) { return a<b?(a<c?a:c):(b<c?b:c); }

int main(int ac, char **av) {
    FILE *f = ac>1 ? fopen(av[1],"r") : stdin;
    char *ln=NULL; size_t sz=0;
    getline(&ln,&sz,f);
    int R=atoi(ln), C, i=0;
    char e=ln[strlen(ln)-4], o=ln[strlen(ln)-3], x=ln[strlen(ln)-2];
    free(ln);
    
    getline(&ln,&sz,f); while(ln[i]&&ln[i]!='\n')i++; C=i;
    char *m=malloc(R*C); int *t=calloc(R*C,4);
    for(int c=0;c<C;c++)m[c]=ln[c];
    for(int r=1;r<R;r++){getline(&ln,&sz,f);for(int c=0;c<C;c++)m[r*C+c]=ln[c];}
    free(ln); if(f!=stdin)fclose(f);
    
    int M=0,Mr=0,Mc=0;
    for(int r=0;r<R;r++)for(int c=0;c<C;c++){
        int p=r*C+c;
        t[p]=(m[p]==o)?0:(r==0||c==0)?1:1+min3(t[p-C],t[p-1],t[p-C-1]);
        if(t[p]>M){M=t[p];Mr=r;Mc=c;}
    }
    free(t);
    
    for(int r=Mr-M+1;r<=Mr;r++)for(int c=Mc-M+1;c<=Mc;c++)m[r*C+c]=x;
    for(int r=0;r<R;r++){for(int c=0;c<C;c++)putchar(m[r*C+c]);putchar('\n');}
    free(m);
}
*/