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
    int buffSize = w*h;

    int* image = (int*) malloc((buffSize)*sizeof(int));
    int* temp_buff = (int*) calloc((size_t)(buffSize),sizeof(int));
    yFromYUV(image_buffer,buffSize,image);
    (env)->ReleaseByteArrayElements(imgData, image_buffer, JNI_ABORT);
    free(image_buffer);

    normalize(image,buffSize,temp_buff);
    GaussianBlur* blur = new GaussianBlur(w,h);
    blur->blur(temp_buff,13,2);
//    blur->blurReduce(&temp_buff,13,2);
    blur->blur(temp_buff,13,2);
    blur->blur(temp_buff,13,2);
    blur->blur(temp_buff,7,2);
    w = blur->getImageWidth();
    h = blur->getImageHeight();
    delete blur;

    EdgeFind* edgeFind = new EdgeFind(w, h);
    edgeFind->computeGradientAngles(temp_buff, dirs);

    env->ReleaseIntArrayElements(preDirs,dirs,JNI_ABORT);
    free(dirs);

    edgeFind->suppressNonMaxima(temp_buff);
    int* thresholds = edgeFind->calcThresholds(threshLow, threshHigh);
    edgeFind->applyThreshold(temp_buff, thresholds[0], thresholds[1]);
    free(thresholds);
    edgeFind->cullShortEdges(5);
    edgeFind->paintEdges(temp_buff);
    for (int i = 0; i < w*h; i++) {
        output_buffer[i] = 0xff000000 | ((temp_buff[i] & 0xff) << 16) | ((temp_buff[i] & 0xff) << 8) | (temp_buff[i] & 0xff);
    }
    output_buffer[buffSize - 2] = w;
    output_buffer[buffSize - 1] = h;

    free(temp_buff);
    delete edgeFind;
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