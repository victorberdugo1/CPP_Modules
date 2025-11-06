#include <stdio.h>
#include <stdlib.h>

char **leer_map(FILE *fp, int *rows, int *cols, char *empty, char *obs, char *full)
{
 if (fscanf(fp, "%d %c %c %c%*c", rows, empty, obs, full) != 4 || *rows <= 0) return NULL;
 if (*empty == *obs || *empty == *full || *obs == *full) return NULL;

 char **map = malloc((*rows) * sizeof(char *));
 if (!map) return NULL;

 int width = 0;
 for (int r = 0; r < *rows; r++) {
  char *line = NULL;
  size_t len = 0;
  ssize_t rd = getline(&line, &len, fp);
  if (rd <= 0) { free(line); for (int k = 0; k < r; k++) free(map[k]); free(map); return NULL; }
  if (line[rd - 1] != '\n') { free(line); for (int k = 0; k < r; k++) free(map[k]); free(map); return NULL; }
  line[--rd] = '\0';
  if (width == 0) {
   if (rd == 0) { free(line); for (int k = 0; k < r; k++) free(map[k]); free(map); return NULL; }
   width = rd;
  } else if (rd != width) {
   free(line); for (int k = 0; k < r; k++) free(map[k]); free(map); return NULL;
  }
  for (int j = 0; j < rd; j++) if (line[j] != *empty && line[j] != *obs) { free(line); for (int k = 0; k < r; k++) free(map[k]); free(map); return NULL; }
  map[r] = line;
 }
 *cols = width;
 return map;
}

void resolver_imprimir(char **map, int rows, int cols, char empty, char obs, char full)
{
 int **dp = calloc(rows, sizeof(int *));
 if (!dp) { fprintf(stderr, "map error\n"); return; }
 for (int i = 0; i < rows; i++) {
  dp[i] = calloc(cols, sizeof(int));
  if (!dp[i]) {
   for (int k = 0; k < i; k++) free(dp[k]);
   free(dp);
   fprintf(stderr, "map error\n");
   return;
  }
 }

 int maxSize = 0, maxR = 0, maxC = 0;
 for (int i = 0; i < rows; i++) {
  for (int j = 0; j < cols; j++) {
   if (map[i][j] == empty) {
    if (i == 0 || j == 0) dp[i][j] = 1;
    else {
     int a = dp[i-1][j], b = dp[i][j-1], c = dp[i-1][j-1];
     int m = a < b ? a : b;
     m = m < c ? m : c;
     dp[i][j] = 1 + m;
    }
    if (dp[i][j] > maxSize) { maxSize = dp[i][j]; maxR = i; maxC = j; }
   } else dp[i][j] = 0;
  }
 }

 if (maxSize > 0) {
  for (int i = maxR - maxSize + 1; i <= maxR; i++)
   for (int j = maxC - maxSize + 1; j <= maxC; j++)
    map[i][j] = full;
 }

 for (int i = 0; i < rows; i++) {
  fputs(map[i], stdout);
  fputc('\n', stdout);
  free(dp[i]);
  free(map[i]);
 }
 free(dp);
 free(map);
}

int main(int argc, char **argv)
{
 if (argc == 1) {
  int rows, cols;
  char empty, obs, full;
  char **map = leer_map(stdin, &rows, &cols, &empty, &obs, &full);
  if (!map) { fprintf(stderr, "map error\n"); return 0; }
  resolver_imprimir(map, rows, cols, empty, obs, full);
 } else {
  for (int i = 1; i < argc; i++) {
   FILE *fp = fopen(argv[i], "r");
   if (!fp) { fprintf(stderr, "map error\n"); }
   else {
    int rows, cols;
    char empty, obs, full;
    char **map = leer_map(fp, &rows, &cols, &empty, &obs, &full);
    if (!map) fprintf(stderr, "map error\n");
    else resolver_imprimir(map, rows, cols, empty, obs, full);
    fclose(fp);
   }
   if (i + 1 < argc) fputc('\n', stdout);
  }
 }
 return 0;
}