#include "GaussianBlur.h"
#include <stdlib.h>

const int GaussianBlur::gausd7s1[] = {1,10,40,64,166};
const int GaussianBlur::gausd7s2[] = {71,131,189,213,995};

void GaussianBlur::blur(int *imgIn, int w, int h, int diameter, int sigma, int *imgOut) {
    switch(diameter) {
        case 3:
        default:
            switch(sigma) {
                case 0:
                default:
                    blur3s0(imgIn,w,h,imgOut);
                    break;
            }
            break;
        case 7:
            const int* gaus;
            switch(sigma) {
                case 1:
                default:
                    gaus = GaussianBlur::gausd7s1;
                    break;
                case 2:
                    gaus = GaussianBlur::gausd7s2;
                    break;
            }
            blur7(imgIn,w,h,gaus,imgOut);
            break;

    }
}

void GaussianBlur::blur3s0(int *imgIn, int w, int h, int *imgOut) {
    int arrSize = w*h;
    for (int p = 0; p < w; p++) {
        imgOut[p] = imgIn[p];
        imgOut[(arrSize - p) - 1] = imgIn[(arrSize - p) - 1];
    }

    //blur horizontal
    int* imgHor = new int[arrSize];
    for (int y = 0; y < h; y++) { //can start at 0 because its doesnt go above current position
        //moves the sides (untouched by blur) into final image
        int yOffs = y * w;
        imgOut[yOffs] = imgIn[yOffs];
        imgOut[yOffs+(w-1)]=imgIn[yOffs+(w-1)];

        for (int x = 1; x < w-1; x++) {
            imgHor[yOffs+x] = (imgIn[yOffs + (x-1)] / 4) +
                              (imgIn[yOffs + x] / 2) +
                              (imgIn[yOffs + (x+1)] / 4);
        }
    }

    //blur vertical
    for (int y = 1; y < (h-1); y++) {
        int yOffs = y * w;
        int yNeg = yOffs - w;
        int yPos = yOffs + w;
        for (int x = 1; x < (w-1); x++) {
            imgOut[yOffs+x] = (imgHor[yNeg+x] / 4) +
                              (imgHor[yOffs+x] / 2) +
                              (imgHor[yPos+x] / 4);
        }
    }
    free(imgHor);
}

void GaussianBlur::blur7(int *imgIn, int w, int h, const int* gaus, int *imgOut) {
    int arrSize = w*h;
    for (int p = 0; p < w; p++) {
        imgOut[p] = imgIn[p];
        imgOut[(arrSize - p) - 1] = imgIn[(arrSize - p) - 1];
    }

    //blur horizontal
    int* imgHor = new int[arrSize];
    for (int y = 0; y < h; y++) { //can start at 0 because its doesnt go above current position
        //moves the sides (untouched by blur) into final image
        int yOffs = y * w;
        imgOut[yOffs+1] = imgIn[yOffs+1];
        imgOut[yOffs+2] = imgIn[yOffs+2];
        imgOut[yOffs+3] = imgIn[yOffs+3];
        imgOut[yOffs+(w-3)]=imgIn[yOffs+(w-3)];
        imgOut[yOffs+(w-2)]=imgIn[yOffs+(w-2)];
        imgOut[yOffs+(w-1)]=imgIn[yOffs+(w-1)];

        for (int x = 3; x < w-3; x++) {
            imgHor[yOffs+x] = ((imgIn[yOffs + (x - 3)] * gaus[0]) +
                              (imgIn[yOffs + (x - 2)] * gaus[1]) +
                              (imgIn[yOffs + (x - 1)] * gaus[2]) +
                              (imgIn[yOffs + (x    )] * gaus[3]) +
                              (imgIn[yOffs + (x + 1)] * gaus[2]) +
                              (imgIn[yOffs + (x + 2)] * gaus[1]) +
                              (imgIn[yOffs + (x + 3)] * gaus[0]))/gaus[4];
        }
    }

    //blur vertical
    for (int y = 3; y < (h-3); y++) {
        int yOffs = y * w;
        int yNeg = yOffs - w;
        int yNng = yNeg - w;
        int yNnn = yNng - w;
        int yPos = yOffs + w;
        int yPps = yPos + w;
        int yPpp = yPps + w;
        for (int x = 1; x < (w-1); x++) {
            imgOut[yOffs+x] = ((imgHor[yNnn+x]*gaus[0]) +
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