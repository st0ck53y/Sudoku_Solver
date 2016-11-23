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
    int* temp_buff2 = (int*)malloc((width*height)*sizeof(int));

//    gaussianBlur(image_buffer, w, h, temp_buff);
    gaussianBlur(image_buffer, w, h, temp_buff);
    int *gradient = (int*) malloc((width*height)*sizeof(int));
    int *direction = (int*)malloc((width*height)*sizeof(int));

    computeGradientAngles(temp_buff, w, h, output_buffer, direction);

    free(image_buffer);
    free(temp_buff);
    free(temp_buff2);
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

double computeXDerivative(int a, int b, int c, int d) {
    return ((b - a) + (d - c))/2;
}

double computeYDerivative(int a, int b, int c, int d) {
    return ((c - a) + (d - b))/2;
}

double toDegrees(double rad) {
    return (rad * 180) / M_PI;
}