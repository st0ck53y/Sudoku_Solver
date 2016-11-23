//
// Created by TomLaptop on 22/11/2016.
//

#include "ImageProcessing.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_st0ck53y_testsudokuapplication_classes_CameraView_nativeCanny(
        JNIEnv *env,
        jobject obj,
        jintArray imgData, jint width, jint height, jint lowerThreshold,
        jint higherThreshold, jobject out) {

    int *output_buffer = (int*) ((env)->GetDirectBufferAddress(out));
    jboolean frame_copy;
    int *image_buffer = (int*) (env)->GetIntArrayElements(imgData, &frame_copy);

    int threshLow  = (int) lowerThreshold;
    int threshHigh = (int) higherThreshold;
    int w = (int) width;
    int h = (int) height;

    int* temp_buff = (int*) malloc((width*height)*sizeof(int));

    gaussianBlur(image_buffer, w, h, temp_buff);
    int *gradient = (int*) malloc((width*height)*sizeof(int));
    int *direction = (int*)malloc((width*height)*sizeof(int));

    computeGradientAngles(temp_buff, w, h, gradient, direction);
    suppressNonMaxima(temp_buff, w, h, gradient, direction);
    applyThreshold(temp_buff, w, h, threshLow, threshHigh, output_buffer);

    free(image_buffer);
    free(temp_buff);
    free(gradient);
    free(direction);
}

void gaussianBlur(int* imgIn, int w, int h, int* output) {
    int arrSize = w*h;
    for (int p = 0; p < w; p++) {
        output[p] = imgIn[p];
        output[(arrSize - p) - 1] = imgIn[(arrSize - p) - 1];
    }

    //blur horizontal
    int* imgHor = new int[arrSize];
    for (int y = 0; y < h; y++) { //can start at 0 because its doesnt go above current position
        //moves the sides (untouched by blur) into final image
        int yOffs = y * w;
        output[yOffs] = imgIn[yOffs];
        output[yOffs+(w-1)]=imgIn[yOffs+(w-1)];

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
            output[yOffs+x] = (imgHor[yNeg+x] / 4) +
                              (imgHor[yOffs+x] / 2) +
                              (imgHor[yPos+x] / 4);
        }
    }
    free(imgHor);
}

void computeGradientAngles(int* imgIn, int w, int h, int* gradient, int* direction) {
    for (int y = 0; y < h - 1; y++) {
        int yOffs = y * w;
        int yPos  = yOffs + w;
        for (int x = 0; x < w - 1; x++) {
            double xd = computeXDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            double yd = computeYDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            gradient[yOffs+x] = (int) round(sqrt((xd*xd)+(yd*yd)));
            direction[yOffs+x] =(int) round(toDegrees(atan(yd/xd)));
        }
    }
}

void suppressNonMaxima(int* imgIn, int w, int h, int* gradient, int* direction) {
    for (int y = 0; y < h-1; y++) {
        int yOffs = y * w;
        for (int x = 0; x < w-1; x++) {
            if (y==0 || x==0) {
                imgIn[yOffs+x] = 0;
                continue;
            }
            int dir = direction[yOffs+x];
            if (dir < -67 || dir > 67) { //Horizontal
                if (gradient[yOffs+x] < gradient[yOffs+x+w] || gradient[yOffs+x] < gradient[yOffs+x-w]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            } else if (dir > -23 && dir < 23) { //Vertical
                if (gradient[yOffs+x] < gradient[yOffs+x+1] || gradient[yOffs+x] < gradient[yOffs+x-1]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            } else if (dir < -22) { //down right - up left
                if (gradient[yOffs+x] < gradient[yOffs+x+(w-1)] || gradient[yOffs+x] < gradient[yOffs+x-(w-1)]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            } else { //up right - down left
                if (gradient[yOffs+x] < gradient[yOffs+x+(w+1)] || gradient[yOffs+x] < gradient[yOffs+x-(w+1)]) {
                    imgIn[yOffs+x] = 0;
                } else {
                    imgIn[yOffs+x] = gradient[yOffs+x];
                }
            }
        }
    }
}

void applyThreshold(int* imgIn, int w, int h, int tL, int tH, int* out) {
    int a = w*h;
    int* lower = (int*) malloc(a*sizeof(int));
    int* higher = (int*) malloc(a* sizeof(int));
    bool* visited = (bool*) malloc(a* sizeof(bool));
    for (int i = 0; i < a; i++) {
        if (imgIn[i] > tH) {
            higher[i] = imgIn[i];
            lower[i] = imgIn[i]; //put in lower too because the line crawling is simpler as a single variable to check
        } else if (imgIn[i] > tL) {
            lower[i] = imgIn[i];
        } else {
            out[i] = 0;
        }
    }

    int cy,cx;
    bool np;
    //hysteresis
    for (int y = 1; y < h - 1; y++) {
        int yOffs = y*w;
        for (int x = 1; x < w - 1; x++) {
            if (visited[yOffs+x]) continue; //skip pixel if already done
            if (higher[yOffs+x] > 0) {
                cy = y;
                cx = x;
                do {
                    np = false;
                    if ((cy*w)+cx >= a || (cy*w)+cx < 0) break;
                    out[(cy*w)+cx] = lower[(cy*w)+cx];
                    visited[(cy*w)+cx] = true;
                    int i = ((cy-1)*w) + (cx-1);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cy -= 1;
                            cx -= 1;
                            np = true;
                            continue;
                        }
                    }
                    i = ((cy-1)*w) + (cx);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cy -= 1;
                            np = true;
                            continue;
                        }
                    }
                    i = ((cy-1)*w) + (cx+1);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cy -= 1;
                            cx += 1;
                            np = true;
                            continue;
                        }
                    }
                    i = (cy)*w + (cx-1);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cx-=1;
                            np = true;
                            continue;
                        }
                    }
                    i = (cy)*w + (cx+1);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cx+=1;
                            np = true;
                            continue;
                        }
                    }
                    i = (cy+1)*w + (cx-1);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cy+=1;
                            cx-=1;
                            np = true;
                            continue;
                        }
                    }
                    i = (cy+1)*w + (cx);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cy+=1;
                            np = true;
                            continue;
                        }
                    }
                    i = (cy+1)*w + (cx+1);
                    if (i < a && i >= 0 && !visited[i]) {
                        if (lower[i] > 0) {
                            cy+=1;
                            cx+=1;
                            np = true;
                        }
                    }
                } while (np);
            }
        }
    }
    free(lower);
    free(higher);
    free(visited);
    for (int i = 0; i < a; i++) {
        if (out[i] != 0) out[i] = 255;
    }
}

double computeXDerivative(int a, int b, int c, int d) {
    return ((b - a) + (d - c))/2;
}

double computeYDerivative(int a, int b, int c, int d) {
    return ((c - a) + (d - b))/2;
}

double toDegrees(double rad) {
    return (rad * 180) / M_PI;
}