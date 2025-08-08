#include "helpers.h"

int **allocate_2d_array(int rows, int cols) {
  int **arr = (int **)calloc(rows, sizeof(int *));
  if (arr == NULL) {
    perror("Ошибка выделения памяти для строк");
    return NULL;
  }
  for (int i = 0; i < rows; i++) {
    arr[i] = (int *)calloc(cols, sizeof(int));
    if (arr[i] == NULL) {
      perror("Ошибка выделения памяти для столбцов");
      for (int j = 0; j < i; j++) {
        free(arr[j]);
      }
      free(arr);
      return NULL;
    }
  }

  return arr;
}

void free_2d_array(int **arr, int rows) {
  if (arr == NULL) return;
  for (int i = 0; i < rows; i++) {
    free(arr[i]);
  }
  free(arr);
}