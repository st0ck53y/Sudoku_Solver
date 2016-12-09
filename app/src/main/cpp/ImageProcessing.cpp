//
// Created by TomLaptop on 22/11/2016.
//

#include "ImageProcessing.h"
#include <stdlib.h>

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

    int* image = (int*) malloc((w*h)*sizeof(int));
    int* temp_buff = (int*) calloc((size_t)(w*h),sizeof(int));
    int* temp_buf2 = (int*) calloc((size_t)(w*h),sizeof(int));
    yFromYUV(image_buffer,w*h,image);
    (env)->ReleaseByteArrayElements(imgData, image_buffer, JNI_ABORT);
    free(image_buffer);

    normalize(image,w*h,temp_buff);
    GaussianBlur::blur(temp_buff,w,h,13,2,temp_buf2);
    GaussianBlur::blur(temp_buf2,w,h,13,2,temp_buff);
    GaussianBlur::blur(temp_buff,w,h,13,2,temp_buf2);
    GaussianBlur::blur(temp_buf2,w,h,13,2,temp_buff);

    free(temp_buf2);
    int* gradient = (int*) malloc((width*height)*sizeof(int));
    int* direction = (int*)malloc((width*height)*sizeof(int));

    computeGradientAngles(temp_buff, w, h, gradient, direction, dirs);
    env->ReleaseIntArrayElements(preDirs,dirs,JNI_ABORT);
    free(dirs);
    suppressNonMaxima(temp_buff, w, h, gradient, direction);
    int* thresholds = calcThresholds(temp_buff, w*h, threshLow, threshHigh);
    applyThreshold(temp_buff, w, h, thresholds[0], thresholds[1], temp_buff);
    free(thresholds);
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

void normalize(int* imgIn, int len, int* imgOut) {
    int* cnt = (int*)calloc(256,sizeof(int));
    for (int i = 0; i < len; i++) {
        cnt[imgIn[i]]++;
    }
    for (int i = 1; i < 256; i++) {
        cnt[i]=cnt[i]+cnt[i-1];
    }
    double var = 255.0 / (double)len;
    for (int i = 0; i < len; i++) {
        imgOut[i] = (int)(cnt[imgIn[i]]*var);
    }
    free(cnt);
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
            direction[yOffs+x] = preCompDir[yIdx<<9|(xIdx&0x1ff)];
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

int* calcThresholds(int* gradient, int len, int dL, int dH) {
    int* t = (int*)malloc(2*sizeof(int));
//    t[0] = dL;
//    t[1] = dH;
//    return t;
    int tot = 0;
    int cnt = 0;
    for (int i = 0; i < len; i++) {
        tot += gradient[i];
        if (gradient[i] > 0) cnt++;
    }
    if (cnt == 0) {
        t[0] = dL;
        t[1] = dH;
        return t;
    }
    int avg = tot / cnt;
    int totL = 0;
    int lCnt = 0;
    int totH = 0;
    int hCnt = 0;
    for (int i = 0; i < len; i++) {
        if (gradient[i] < avg) {
            totL += gradient[i];
            if (gradient[i] > 0) lCnt++;
        } else {
            totH += gradient[i];
            hCnt++;
        }
    }
    if (lCnt == 0) {
        t[0] = dL;
    } else {
        t[0] = (3*totL/lCnt);
    }
    if (hCnt == 0) {
        t[1] = dH;
    } else{
        t[1] = (3*totH/hCnt);
    }
    return t;
}