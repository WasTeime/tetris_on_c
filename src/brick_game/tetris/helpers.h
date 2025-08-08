#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>

int **allocate_2d_array(int rows, int cols);
void free_2d_array(int **arr, int rows);

#endif