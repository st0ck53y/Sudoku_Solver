#ifndef SUDOKU_SOLVER_GAUSSIANBLUR_H
#define SUDOKU_SOLVER_GAUSSIANBLUR_H


class GaussianBlur {
public:
    GaussianBlur(int w, int h);
    ~GaussianBlur();
    void blur(int* img, int diameter, int sigma);
    void blurReduce(int** img, int diameter, int sigma);
    int getImageWidth();
    int getImageHeight();
private:
    void blur13(int* img, const int* gaus);
    void blur7(int* img, const int* gaus);
    void blur3s0(int* img);
    void blurReduce13(int** img, const int* gaus);
private:
    static const int gausd7s1[];
    static const int gausd7s2[];
    static const int gausd13s2[];
    int m_w;
    int m_h;
};


#endif //SUDOKU_SOLVER_GAUSSIANBLUR_H
