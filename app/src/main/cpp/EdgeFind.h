//
// Created by TomLaptop on 14/01/2017.
//

#include <stdint.h>
#include <math.h>

#ifndef SUDOKU_SOLVER_EDGE_H
#define SUDOKU_SOLVER_EDGE_H

struct Line {
    int p;
    Line* next;
};
class EdgeFind{
public:
    EdgeFind(int imageWidth, int imageHeight);
    ~EdgeFind();
    void computeGradientAngles(int* imgIn, int* preCompDir);
    void suppressNonMaxima(int* imgIn);
    void** applyThreshold(int* imgIn, int tL, int tH);
    int* calcThresholds(int dL, int dH);
    void** cullShortEdges(void** edges, int thresh);
    void paintEdges(int* img, void** edges);
    int* getImageGradients();
    int* getImageDirections();
private:
    int computeXDerivative(int a, int b, int c, int d);
    int computeYDerivative(int a, int b, int c, int d);
    void* traverseLine(bool* visited, int* lower, int y, int x);
    bool walked(bool* visited, int* lower, int imgLen, int cn, int& cy, int& cx, int yc, int xc);
    void appendToLine(Line*& cur, int val);
    void prependToLine(Line*& root, int val);

private:
    int w;
    int h;
    int* gradient;
    int* direction;
};

#endif //SUDOKU_SOLVER_EDGE_H