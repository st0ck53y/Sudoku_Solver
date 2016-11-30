//
// Created by TomLaptop on 22/11/2016.
//

#include <jni.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#ifndef SUDOKU_SOLVER_IMAGEPROCESSING_H
#define SUDOKU_SOLVER_IMAGEPROCESSING_H

#endif //SUDOKU_SOLVER_IMAGEPROCESSING_H

void yFromYUV(jbyte* imgIn, int len, int* imgOut);
void yNormFromYUV(jbyte* imgIn, int len, int* imgOut);
void gaussianBlur(int* imgIn, int w, int h, int* output);
void computeGradientAngles(int* imgIn, int w, int h, int* gradient, int* direction, int* preCompDir);
void suppressNonMaxima(int* imgIn, int w, int h, int* gradient, int* direction);
void applyThreshold(int* imgIn, int w, int h, int tL, int tH, int* out);
int computeXDerivative(int a, int b, int c, int d);
int computeYDerivative(int a, int b, int c, int d);

