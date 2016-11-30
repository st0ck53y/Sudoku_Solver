//
// Created by TomLaptop on 22/11/2016.
//

#include "ImageProcessing.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_st0ck53y_testsudokuapplication_helper_NativeHelper_nativeCanny(
        JNIEnv *env,
        jclass obj,
        jbyteArray imgData, jint width, jint height, jintArray preDirs,
        jint lowerThreshold,
        jint higherThreshold, jobject out) {

    int *output_buffer = (int*) ((env)->GetDirectBufferAddress(out));
    jboolean frame_copy;
    jbyte* image_buffer = (env)->GetByteArrayElements(imgData, &frame_copy);
    jboolean dirs_copy;
    int* dirs = (env)->GetIntArrayElements(preDirs, &dirs_copy);

    int threshLow  = (int) lowerThreshold;
    int threshHigh = (int) higherThreshold;
    int w = (int) width;
    int h = (int) height;

    int* temp_buff = (int*) malloc((width*height)*sizeof(int));
    int* temp_buf2 = (int*) malloc((width*height)*sizeof(int));
    yFromYUV(image_buffer,w*h,temp_buf2);
    (env)->ReleaseByteArrayElements(imgData, image_buffer, JNI_ABORT);
    free(image_buffer);
    gaussianBlur(temp_buf2, w, h, temp_buff);
    free(temp_buf2);
    int* gradient = (int*) malloc((width*height)*sizeof(int));
    int* direction = (int*)malloc((width*height)*sizeof(int));

    computeGradientAngles(temp_buff, w, h, gradient, direction, dirs);
    env->ReleaseIntArrayElements(preDirs,dirs,JNI_ABORT);
    free(dirs);
    suppressNonMaxima(temp_buff, w, h, gradient, direction);
    applyThreshold(temp_buff, w, h, threshLow, threshHigh, temp_buff);

    for (int i = 0; i < w*h; i++) {
        output_buffer[i] = 0xff000000 | ((temp_buff[i] & 0xff) << 16) | ((temp_buff[i] & 0xff) << 8) | (temp_buff[i] & 0xff);
    }

    free(temp_buff);
    free(gradient);
    free(direction);
}

void yFromYUV(jbyte* imgIn, int len, int* imgOut) {
    for (int i = 0; i < len; i++) {
        imgOut[i] = imgIn[i]&0xff;
    }
}

void yNormFromYUV(jbyte* imgIn, int len, int* imgOut) {
    int* cnt = (int*)malloc(256*sizeof(int));
    for (int i = 0; i < len; i++) {
        imgOut[i] = imgIn[i]&0xff;
        cnt[imgOut[i]]++;
    }
    for (int i = 0; i < 256; i++) {
        cnt[i]=cnt[i]+cnt[i-1];
    }
    double var = 255 / len;
    for (int i = 0; i < len; i++) {
        imgOut[i] = (int)(cnt[imgOut[i]]*var);
    }
    free(cnt);
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

void computeGradientAngles(int* imgIn, int w, int h, int* gradient, int* direction, int* preCompDir) {
    for (int y = 0; y < h - 1; y++) {
        int yOffs = y * w;
        int yPos  = yOffs + w;
        for (int x = 0; x < w - 1; x++) {
            int xd = computeXDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            int yd = computeYDerivative(imgIn[yOffs + x], imgIn[yOffs + x + 1], imgIn[yPos + x], imgIn[yPos + x + 1]);
            gradient[yOffs+x] = (int) round(sqrt((xd*xd)+(yd*yd)));
            int yIdx = (yd & 0xff) | (yd<0?1<<8:0);
            int xIdx = (xd & 0xff) | (xd<0?1<<8:0);
            direction[yOffs+x] = preCompDir[yIdx<<9|xIdx&0x1ff];
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

int computeXDerivative(int a, int b, int c, int d) {
    return ((b - a) + (d - c))/2;
}

int computeYDerivative(int a, int b, int c, int d) {
    return ((c - a) + (d - b))/2;
}