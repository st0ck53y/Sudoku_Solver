//
// Created by TomLaptop on 22/11/2016.
//

#include <jni.h>
#include <stdint.h>
#include <math.h>
#include "GaussianBlur.h"

#ifndef SUDOKU_SOLVER_IMAGEPROCESSING_H
#define SUDOKU_SOLVER_IMAGEPROCESSING_H

#endif //SUDOKU_SOLVER_IMAGEPROCESSING_H

void yFromYUV(jbyte* imgIn, int len, int* imgOut);
void normalize(int* imgIn, int len, int* imgOut);
void computeGradientAngles(int* imgIn, int w, int h, int* gradient, int* direction, int* preCompDir);
void suppressNonMaxima(int* imgIn, int w, int h, int* gradient, int* direction);
void applyThreshold(int* imgIn, int w, int h, int tL, int tH, int* out);
int computeXDerivative(int a, int b, int c, int d);
int computeYDerivative(int a, int b, int c, int d);
int* calcThresholds(int* gradient, int len, int dL, int dH);
