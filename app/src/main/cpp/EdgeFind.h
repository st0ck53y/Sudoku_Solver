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

    void simplifyDirections();
    void normalizeGradients();
    void thresholdGradients();
    void thresholdGradients(int thresh);
    void findAnchors();
    void findAnchors(int thresh, int interval);
    void joinAnchors();

    void cullShortEdges(int thresh);
    void** cullShortEdges(void** edges, int thresh);

    void paintEdges(int* img);
    void paintEdges(int* img, void** edges);
    int* getImageGradients();
    int* getImageDirections();
    int* getAnchors();
    void setAnchors(int* anchors); //TODO used for debugging anchor join only
    void** getImageEdges();
private:
    int computeXDerivative(int a, int b, int c, int d);
    int computeYDerivative(int a, int b, int c, int d);
    void insertToLine(Line*& cur, int val);
    void prependToLine(Line*& root, int val);

    void mergeLines(Line*& ro, Line* rt);
    void walkLine(int cPos, bool left, bool up, Line* line);
    int goLeft(int x, int y, bool& up, Line* line);
    int goRight(int x, int y, bool& up, Line* line);
    int goUp(int x, int y, bool& left, Line* line);
    int goDown(int x, int y, bool& left, Line* line);

private:
    int w;
    int h;
    int* m_gradient;
    int* m_direction;
    bool* m_simpDirs; //l/r==false,u/d==true
    int* m_anchors;
    int m_ancMax;
    void** m_edges;
    int m_edgeMax;

    bool* m_visited;
};

#endif //SUDOKU_SOLVER_EDGE_H