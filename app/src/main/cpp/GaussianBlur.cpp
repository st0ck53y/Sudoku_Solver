#include "GaussianBlur.h"
#include <stdlib.h>

const int GaussianBlur::gausd7s1[] = {1,10,40,64,166};
const int GaussianBlur::gausd7s2[] = {71,131,189,213,995};
const int GaussianBlur::gausd13s2[] = {1,5,14,33,60,88,99,501};

GaussianBlur::GaussianBlur(int w, int h) {
    m_w = w;
    m_h = h;
}

GaussianBlur::~GaussianBlur() {

}

int GaussianBlur::getImageWidth() { return m_w; }
int GaussianBlur::getImageHeight() { return m_h; }

void GaussianBlur::blur(int *img, int diameter, int sigma) {
    switch(diameter) {
        case 3:
        default:
            switch(sigma) {
                case 0:
                default:
                    blur3s0(img);
                    break;
            }
            break;
        case 7:
            const int* gaus;
            switch(sigma) {
                case 1:
                default:
                    gaus = gausd7s1;
                    break;
                case 2:
                    gaus = gausd7s2;
                    break;
            }
            blur7(img, gaus);
            break;
        case 13:
            blur13(img, gausd13s2);
            break;
    }
}

void GaussianBlur::blurReduce(int** img, int diameter, int sigma) {
    switch (diameter) {
        case 13:
            switch(sigma) {
                case 2:
                    blurReduce13(img,gausd13s2);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void GaussianBlur::blur3s0(int* img) {
    int arrSize = m_w*m_h;

    //blur horizontal
    int* imgHor = new int[arrSize];
    for (int y = 0; y < m_h; y++) { //can start at 0 because its doesnt go above current position
        int yOffs = y * m_w;
        for (int x = 1; x < m_w-1; x++) {
            imgHor[yOffs+x] = (img[yOffs + (x-1)] / 4) +
                              (img[yOffs + x] / 2) +
                              (img[yOffs + (x+1)] / 4);
        }
    }

    //blur vertical
    for (int y = 1; y < (m_h-1); y++) {
        int yOffs = y * m_w;
        int yNeg = yOffs - m_w;
        int yPos = yOffs + m_w;
        for (int x = 1; x < (m_w-1); x++) {
            img[yOffs+x] = (imgHor[yNeg+x] / 4) +
                              (imgHor[yOffs+x] / 2) +
                              (imgHor[yPos+x] / 4);
        }
    }
    free(imgHor);
}

void GaussianBlur::blur7(int* img, const int* gaus) {
    int arrSize = m_w*m_h;

    //blur horizontal
    int* imgHor = new int[arrSize];
    for (int y = 0; y < m_h; y++) { //can start at 0 because its doesnt go above current position
        int yOffs = y * m_w;

        for (int x = 3; x < m_w-3; x++) {
            imgHor[yOffs+x] = ((img[yOffs + (x - 3)] * gaus[0]) +
                              (img[yOffs + (x - 2)] * gaus[1]) +
                              (img[yOffs + (x - 1)] * gaus[2]) +
                              (img[yOffs + (x    )] * gaus[3]) +
                              (img[yOffs + (x + 1)] * gaus[2]) +
                              (img[yOffs + (x + 2)] * gaus[1]) +
                              (img[yOffs + (x + 3)] * gaus[0]))/gaus[4];
        }
    }

    //blur vertical
    for (int y = 3; y < (m_h-3); y++) {
        int yOffs = y * m_w;
        int yNeg = yOffs - m_w;
        int yNng = yNeg - m_w;
        int yNnn = yNng - m_w;
        int yPos = yOffs + m_w;
        int yPps = yPos + m_w;
        int yPpp = yPps + m_w;
        for (int x = 1; x < (m_w-1); x++) {
            img[yOffs+x] = ((imgHor[yNnn+x]*gaus[0]) +
                    (imgHor[yNng+x]*gaus[1]) +
                    (imgHor[yNeg+x]*gaus[2]) +
                    (imgHor[yOffs+x]*gaus[3]) +
                    (imgHor[yPos+x]*gaus[2]) +
                    (imgHor[yPps+x]*gaus[1]) +
                    (imgHor[yPpp+x]*gaus[0]))/gaus[4];
        }
    }
    free(imgHor);
}

void GaussianBlur::blur13(int* img, const int* gaus) {
    int arrSize = m_w*m_h;
    //TODO handle edges

    int* imgHor = new int[arrSize];
    //blur horizontal
    for (int y = 0; y < m_h; y++) {
        int yOffs = y*m_w;
        for (int x = 6; x < (m_w - 6); x++) {
            imgHor[yOffs+x] = ((img[yOffs + (x-6)]*gaus[0])+
                    (img[yOffs + (x-5)]*gaus[1])+
                    (img[yOffs + (x-4)]*gaus[2])+
                    (img[yOffs + (x-3)]*gaus[3])+
                    (img[yOffs + (x-2)]*gaus[4])+
                    (img[yOffs + (x-1)]*gaus[5])+
                    (img[yOffs + (x  )]*gaus[6])+
                    (img[yOffs + (x+1)]*gaus[5])+
                    (img[yOffs + (x+2)]*gaus[4])+
                    (img[yOffs + (x+3)]*gaus[3])+
                    (img[yOffs + (x+4)]*gaus[2])+
                    (img[yOffs + (x+5)]*gaus[1])+
                    (img[yOffs + (x+6)]*gaus[0]))/gaus[7];
        }
    }

    //0 edges
    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < m_w; x++) {
            img[y*m_w+x] = 0;
            img[(m_w*m_h)-(y*m_w+x)-1]=0;
        }
    }

    //blur vertical
    for (int y = 6; y < (m_h-6); y++) {
        int yOffs = y * m_w;
        for (int x = 6; x < (m_w-6); x++) {
            img[yOffs+x] = (
                    (imgHor[(yOffs - (6*m_w))+x]*gaus[0])+
                    (imgHor[(yOffs - (5*m_w))+x]*gaus[1])+
                    (imgHor[(yOffs - (4*m_w))+x]*gaus[2])+
                    (imgHor[(yOffs - (3*m_w))+x]*gaus[3])+
                    (imgHor[(yOffs - (2*m_w))+x]*gaus[4])+
                    (imgHor[(yOffs - (  m_w))+x]*gaus[5])+
                    (imgHor[(yOffs          )+x]*gaus[6])+
                    (imgHor[(yOffs + (  m_w))+x]*gaus[5])+
                    (imgHor[(yOffs + (2*m_w))+x]*gaus[4])+
                    (imgHor[(yOffs + (3*m_w))+x]*gaus[3])+
                    (imgHor[(yOffs + (4*m_w))+x]*gaus[2])+
                    (imgHor[(yOffs + (5*m_w))+x]*gaus[1])+
                    (imgHor[(yOffs + (6*m_w))+x]*gaus[0]))/gaus[7];
        }
        for (int x = 0; x < 6; x++) {
            img[yOffs+x] = 0;
            img[(yOffs+m_w)-1+x]=0;
        }
    }
    free(imgHor);
}

void GaussianBlur::blurReduce13(int** img, const int* gaus) { ;
    int* imgHor = (int*)malloc(m_w*m_h*sizeof(int));
    for (int y = 0; y < m_h; y++) {
        int yOffs = y*m_w;
        for (int x = 7; x < (m_w-6); x++){
            imgHor[yOffs+x] = ((img[0][yOffs + (x-6)]*gaus[0])+
                               (img[0][yOffs + (x-5)]*gaus[1])+
                               (img[0][yOffs + (x-4)]*gaus[2])+
                               (img[0][yOffs + (x-3)]*gaus[3])+
                               (img[0][yOffs + (x-2)]*gaus[4])+
                               (img[0][yOffs + (x-1)]*gaus[5])+
                               (img[0][yOffs + (x  )]*gaus[6])+
                               (img[0][yOffs + (x+1)]*gaus[5])+
                               (img[0][yOffs + (x+2)]*gaus[4])+
                               (img[0][yOffs + (x+3)]*gaus[3])+
                               (img[0][yOffs + (x+4)]*gaus[2])+
                               (img[0][yOffs + (x+5)]*gaus[1])+
                               (img[0][yOffs + (x+6)]*gaus[0]))/gaus[7];
        }
    }
//    free(img);
    //calculate new width and height after reduction
    int nW = 0;
    for (int x = 1; x < m_w; x+=3) nW++;
    int nH = 0;
    for (int y = 1; y < m_h; y+=3) nH++;
    int* newImage = (int*) malloc((nW*nH)*sizeof(int));
    int pos = 0;
    for (int y = 7; y < (m_h-6); y+=3) {
        int yOffs = y*m_w;
        pos += 4;
        for (int x = 7; x < (m_w-6); x+=3) {
            newImage[pos++] = (
                       (imgHor[(yOffs - (6*m_w))+x]*gaus[0])+
                       (imgHor[(yOffs - (5*m_w))+x]*gaus[1])+
                       (imgHor[(yOffs - (4*m_w))+x]*gaus[2])+
                       (imgHor[(yOffs - (3*m_w))+x]*gaus[3])+
                       (imgHor[(yOffs - (2*m_w))+x]*gaus[4])+
                       (imgHor[(yOffs - (  m_w))+x]*gaus[5])+
                       (imgHor[(yOffs          )+x]*gaus[6])+
                       (imgHor[(yOffs + (  m_w))+x]*gaus[5])+
                       (imgHor[(yOffs + (2*m_w))+x]*gaus[4])+
                       (imgHor[(yOffs + (3*m_w))+x]*gaus[3])+
                       (imgHor[(yOffs + (4*m_w))+x]*gaus[2])+
                       (imgHor[(yOffs + (5*m_w))+x]*gaus[1])+
                       (imgHor[(yOffs + (6*m_w))+x]*gaus[0]))/gaus[7];
        }
        pos += 4;
    }
    m_w = nW;
    m_h = nH;
    free(imgHor);
    *img = newImage;
}