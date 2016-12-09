#ifndef SUDOKU_SOLVER_GAUSSIANBLUR_H
#define SUDOKU_SOLVER_GAUSSIANBLUR_H


class GaussianBlur {
public:
    static void blur(int* imgIn, int w, int h, int diameter, int sigma, int* imgOut);

private:
    static void blur13(int* imgIn, int w, int h, const int* gaus, int* imgOut);
    static void blur7(int* imgIn, int w, int h, const int* gaus, int* imgOut);
    static void blur3s0(int* imgIn, int w, int h, int* imgOut);

    static const int gausd7s1[];
    static const int gausd7s2[];
    static const int gausd13s2[];
};


#endif //SUDOKU_SOLVER_GAUSSIANBLUR_H
