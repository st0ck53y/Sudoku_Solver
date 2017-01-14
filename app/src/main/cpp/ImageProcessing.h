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

struct Line {
    int p;
    Line* next;
};

void yFromYUV(jbyte* imgIn, int len, int* imgOut);
void normalize(int* imgIn, int len, int* imgOut);
void computeGradientAngles(int* imgIn, int w, int h, int* gradient, int* direction, int* preCompDir);
void suppressNonMaxima(int* imgIn, int w, int h, int* gradient, int* direction);
void** applyThreshold(int* imgIn, int w, int h, int tL, int tH);
int computeXDerivative(int a, int b, int c, int d);
int computeYDerivative(int a, int b, int c, int d);
int* calcThresholds(int* gradient, int len, int dL, int dH);
void** cullShortEdges(void** edges, int thresh);
void* traverseLine(bool* visited, int* lower, int w, int h, int y, int x);
bool walked(bool* visited, int* lower, int imgLen, int cn, int& cy, int& cx, int yc, int xc);
void appendToLine(Line*& cur, int val);
void prependToLine(Line*& root, int val);
void paintEdges(int* img, int imgLen, void** edges);