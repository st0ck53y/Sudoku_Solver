//
// Created by TomLaptop on 22/11/2016.
//

#include <jni.h>
#include <stdint.h>
#include <math.h>
#include "GaussianBlur.h"
#include "EdgeFind.h"

#ifndef SUDOKU_SOLVER_IMAGEPROCESSING_H
#define SUDOKU_SOLVER_IMAGEPROCESSING_H

#endif //SUDOKU_SOLVER_IMAGEPROCESSING_H

void yFromYUV(jbyte* imgIn, int len, int* imgOut);
void normalize(int* imgIn, int len, int* imgOut);