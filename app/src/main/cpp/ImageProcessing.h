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

void gaussianBlur(int* imgIn, int w, int h, int* output);
void computeGradientAngles(int* imgIn, int w, int h, int* gradient, int* direction);
double computeXDerivative(int a, int b, int c, int d);
double computeYDerivative(int a, int b, int c, int d);
double toDegrees(double rad);
