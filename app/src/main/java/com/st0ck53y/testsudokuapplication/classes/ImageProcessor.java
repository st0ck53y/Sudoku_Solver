package com.st0ck53y.testsudokuapplication.classes;

public class ImageProcessor {

    //TODO handle borders
    // ~448 divisions+multiplications for a 10x10 image
    public static int[] gaussianBlur3(int[] imgIn, int w, int h) {
        int arrSize = w*h;
        int[] imgOut = new int[arrSize];
        for (int p = 0; p < w; p++) {
            imgOut[p] = imgIn[p];
            imgOut[(arrSize - p) - 1] = imgIn[(arrSize - p) - 1];
        }

        //blur horizontal
        int[] imgHor = new int[arrSize];
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

        return imgOut;
    }

    // ~ 737 divisions+multiplications for a 10x10 image ... eg almost half the loops, but twice the operations
    /**
     * Blurs a single channel image using gaussian method with kernel size of 3
     *
     * @param imgIn array of pixel data for a single channel
     * @param width width of image
     * @param height height of image
     * @param diameter
     * @return single channel image blurred
     */
    public static int[] gaussianBlurNS(int[] imgIn, int width, int height, int diameter) {
        int arrSize = width*height;
        int[] imgOut = new int[arrSize];
        for (int p = 0; p < width; p++) {
            imgOut[p] = imgIn[p];
            imgOut[(arrSize - p) - 1] = imgIn[(arrSize - p) - 1];
        }

        for (int y = 1; y < height - 1; y++) {
            int yOffs = y * width;
            imgOut[yOffs] = imgIn[yOffs];
            imgOut[yOffs+(width-1)]=imgIn[yOffs+(width-1)];
            int yNeg = yOffs - width;
            int yPos = yOffs + width;
            for (int x = 1; x < width - 1; x++) {
                int border = (diameter -1)/2;
                if (y < border || y >= height-border || x < border || x >= width-border) {
                    imgOut[yOffs+x] = imgIn[yOffs+x];
                } else {
                    switch(diameter) {
                        case 3:
                            imgOut[yOffs+x] = blurPixel3(imgIn, x, yOffs, yNeg, yPos);
                            break;
                        case 5:
                            imgOut[yOffs+x] = blurPixel5(imgIn,x,yOffs,width);
                            break;
                        case 7:
                            imgOut[yOffs+x] = blurPixel7(imgIn,x,yOffs,width);
                            break;
                    }
                }
            }
        }
        return imgOut;
    }

    static int blurPixel3(int[] imgIn, int x, int row, int prevRow, int nextRow) {
        double a = (imgIn[prevRow + x - 1]);
        double b = (imgIn[prevRow + x] * 2);
        double c = (imgIn[prevRow + x + 1] );
        double d = (imgIn[row + x - 1]  * 2);
        double e = (imgIn[row + x] * 4);
        double f = (imgIn[row + x + 1] * 2);
        double g = (imgIn[nextRow + x - 1] );
        double h = (imgIn[nextRow + x] * 2);
        double i = (imgIn[nextRow + x + 1]);
//        System.out.println(d);
        return (int)(a + b + c + d + e + f + g + h + i)/16;
    }

    //top right corner of grid as Gaussian is symmetric + divisor
    static double[] gausR5SU = {1,4,6,4,16,24,6,24,36,256};
    static double[] gausR5S0_75 = {0.000499,0.005137,0.011068,0.005137,0.052872,0.113921,0.011068,0.113921,0.245461,1};
    static double[] gausR5S2_50 = {0.028672,0.036333,0.039317,0.036333,0.046042,0.049824,0.039317,0.049824,0.053916,1};
    static double[] gausR7S3_50 = {0.013347,0.016346,0.018460,0.019223,0.016346,0.020019,0.022608,0.023543,0.018460,0.022608,0.025532,0.026588,0.019223,0.023543,0.026588,0.027688,1};

    static int blurPixel5(int[] imgIn, int x, int yOffs, int w) {
        double[] pix = new double[25];
        double[] gaus = gausR5S2_50;
        pix[ 0] = (imgIn[(yOffs - (2 * w) + (x - 2))] * gaus[0]);
        pix[ 1] = (imgIn[(yOffs - (2 * w) + (x - 1))] * gaus[1]);
        pix[ 2] = (imgIn[(yOffs - (2 * w) + (x    ))] * gaus[2]);
        pix[ 3] = (imgIn[(yOffs - (2 * w) + (x + 1))] * gaus[1]);
        pix[ 4] = (imgIn[(yOffs - (2 * w) + (x + 2))] * gaus[0]);
        pix[ 5] = (imgIn[(yOffs - (    w) + (x - 2))] * gaus[3]);
        pix[ 6] = (imgIn[(yOffs - (    w) + (x - 1))] * gaus[4]);
        pix[ 7] = (imgIn[(yOffs - (    w) + (x    ))] * gaus[5]);
        pix[ 8] = (imgIn[(yOffs - (    w) + (x + 1))] * gaus[4]);
        pix[ 9] = (imgIn[(yOffs - (    w) + (x + 2))] * gaus[3]);
        pix[10] = (imgIn[(yOffs           + (x - 2))] * gaus[6]);
        pix[11] = (imgIn[(yOffs           + (x - 1))] * gaus[7]);
        pix[12] = (imgIn[(yOffs           + (x    ))] * gaus[8]);
        pix[13] = (imgIn[(yOffs           + (x + 1))] * gaus[7]);
        pix[14] = (imgIn[(yOffs           + (x + 2))] * gaus[6]);
        pix[15] = (imgIn[(yOffs + (    w) + (x - 2))] * gaus[3]);
        pix[16] = (imgIn[(yOffs + (    w) + (x - 1))] * gaus[4]);
        pix[17] = (imgIn[(yOffs + (    w) + (x    ))] * gaus[5]);
        pix[18] = (imgIn[(yOffs + (    w) + (x + 1))] * gaus[4]);
        pix[19] = (imgIn[(yOffs + (    w) + (x + 2))] * gaus[3]);
        pix[20] = (imgIn[(yOffs + (2 * w) + (x - 2))] * gaus[0]);
        pix[21] = (imgIn[(yOffs + (2 * w) + (x - 1))] * gaus[1]);
        pix[22] = (imgIn[(yOffs + (2 * w) + (x    ))] * gaus[2]);
        pix[23] = (imgIn[(yOffs + (2 * w) + (x + 1))] * gaus[1]);
        pix[24] = (imgIn[(yOffs + (2 * w) + (x + 2))] * gaus[0]);
        double sum = 0;
        for (int i = 0; i < 25; i++) {
            sum += pix[i];
        }
        return (int)(sum/gaus[9]);
    }

    static int blurPixel7(int[] imgIn, int x, int yOffs, int w) {
        double[] pix = new double[49];
        double[] gaus = gausR7S3_50;
        pix[ 0] = (imgIn[yOffs - (3 * w) + (x - 3)] * gaus[ 0]);
        pix[ 1] = (imgIn[yOffs - (3 * w) + (x - 2)] * gaus[ 1]);
        pix[ 2] = (imgIn[yOffs - (3 * w) + (x - 1)] * gaus[ 2]);
        pix[ 3] = (imgIn[yOffs - (3 * w) + (x    )] * gaus[ 3]);
        pix[ 4] = (imgIn[yOffs - (3 * w) + (x + 1)] * gaus[ 2]);
        pix[ 5] = (imgIn[yOffs - (3 * w) + (x + 2)] * gaus[ 1]);
        pix[ 6] = (imgIn[yOffs - (3 * w) + (x + 3)] * gaus[ 0]);
        pix[ 7] = (imgIn[yOffs - (2 * w) + (x - 3)] * gaus[ 4]);
        pix[ 8] = (imgIn[yOffs - (2 * w) + (x - 2)] * gaus[ 5]);
        pix[ 9] = (imgIn[yOffs - (2 * w) + (x - 1)] * gaus[ 6]);
        pix[10] = (imgIn[yOffs - (2 * w) + (x    )] * gaus[ 7]);
        pix[11] = (imgIn[yOffs - (2 * w) + (x + 1)] * gaus[ 6]);
        pix[12] = (imgIn[yOffs - (2 * w) + (x + 2)] * gaus[ 5]);
        pix[13] = (imgIn[yOffs - (2 * w) + (x + 3)] * gaus[ 4]);
        pix[14] = (imgIn[yOffs - (    w) + (x - 3)] * gaus[ 8]);
        pix[15] = (imgIn[yOffs - (    w) + (x - 2)] * gaus[ 9]);
        pix[16] = (imgIn[yOffs - (    w) + (x - 1)] * gaus[10]);
        pix[17] = (imgIn[yOffs - (    w) + (x    )] * gaus[11]);
        pix[18] = (imgIn[yOffs - (    w) + (x + 1)] * gaus[10]);
        pix[19] = (imgIn[yOffs - (    w) + (x + 2)] * gaus[ 9]);
        pix[20] = (imgIn[yOffs - (    w) + (x + 3)] * gaus[ 8]);
        pix[21] = (imgIn[yOffs           + (x - 3)] * gaus[12]);
        pix[22] = (imgIn[yOffs           + (x - 2)] * gaus[13]);
        pix[23] = (imgIn[yOffs           + (x - 1)] * gaus[14]);
        pix[24] = (imgIn[yOffs           + (x    )] * gaus[15]);
        pix[25] = (imgIn[yOffs           + (x + 1)] * gaus[14]);
        pix[26] = (imgIn[yOffs           + (x + 2)] * gaus[13]);
        pix[27] = (imgIn[yOffs           + (x + 3)] * gaus[12]);
        pix[28] = (imgIn[yOffs + (    w) + (x - 3)] * gaus[ 8]);
        pix[29] = (imgIn[yOffs + (    w) + (x - 2)] * gaus[ 9]);
        pix[30] = (imgIn[yOffs + (    w) + (x - 1)] * gaus[10]);
        pix[31] = (imgIn[yOffs + (    w) + (x    )] * gaus[11]);
        pix[32] = (imgIn[yOffs + (    w) + (x + 1)] * gaus[10]);
        pix[33] = (imgIn[yOffs + (    w) + (x + 2)] * gaus[ 9]);
        pix[34] = (imgIn[yOffs + (    w) + (x + 3)] * gaus[ 8]);
        pix[35] = (imgIn[yOffs + (2 * w) + (x - 3)] * gaus[ 4]);
        pix[36] = (imgIn[yOffs + (2 * w) + (x - 2)] * gaus[ 5]);
        pix[37] = (imgIn[yOffs + (2 * w) + (x - 1)] * gaus[ 6]);
        pix[38] = (imgIn[yOffs + (2 * w) + (x    )] * gaus[ 7]);
        pix[39] = (imgIn[yOffs + (2 * w) + (x + 1)] * gaus[ 6]);
        pix[40] = (imgIn[yOffs + (2 * w) + (x + 2)] * gaus[ 5]);
        pix[41] = (imgIn[yOffs + (2 * w) + (x + 3)] * gaus[ 4]);
        pix[42] = (imgIn[yOffs + (3 * w) + (x - 3)] * gaus[ 0]);
        pix[43] = (imgIn[yOffs + (3 * w) + (x - 2)] * gaus[ 1]);
        pix[44] = (imgIn[yOffs + (3 * w) + (x - 1)] * gaus[ 2]);
        pix[45] = (imgIn[yOffs + (3 * w) + (x    )] * gaus[ 3]);
        pix[46] = (imgIn[yOffs + (3 * w) + (x + 1)] * gaus[ 2]);
        pix[47] = (imgIn[yOffs + (3 * w) + (x + 2)] * gaus[ 1]);
        pix[48] = (imgIn[yOffs + (3 * w) + (x + 3)] * gaus[ 0]);
        double sum = 0;
        for (int i = 0; i < 49; i++) {
            sum += pix[i];
        }
        return (int)(sum/gaus[16]);
    }

    /**
     *  Calculates the pixel first order differential of the image, returning the differential unmodified
     *
     * @param imgIn array of pixel data for a single channel
     * @param width width of image
     * @param height height of image
     * @return array of pixel differentials ~(vertical + horizontal / 2)
     */
    public static int[] sobelDifferential(int[] imgIn, int width, int height) {
        int arrSize = width*height;
        int[] imgOut = new int[arrSize];
        for (int p = 0; p < width; p++) {
            imgOut[p] = imgIn[p];
            imgOut[(arrSize - p) - 1] = imgIn[(arrSize - p) - 1];
        }

        for (int y = 1; y < height - 1; y++) {
            imgOut[(y*width)] = imgIn[(y*width)];
            imgOut[(y*width)+(width-1)]=imgIn[(y*width)+(width-1)];
            int yOffs = y * width;
            int yNeg = yOffs - width;
            int yPos = yOffs + width;
            for (int x = 1; x < width - 1; x++) {
                int hor = (-(imgIn[yNeg + x - 1])) +
                            (imgIn[yNeg + x + 1]) -
                            (imgIn[yOffs + x - 1] << 1) +
                            (imgIn[yOffs + x + 1] << 1) -
                            (imgIn[yPos + x - 1]) +
                            (imgIn[yPos + x + 1]);

                int ver = (-(imgIn[yNeg + x - 1])) -
                            (imgIn[yNeg + x] << 1) -
                            (imgIn[yNeg + x + 1]) +
                            (imgIn[yPos + x - 1]) +
                            (imgIn[yPos + x] << 1) +
                            (imgIn[yPos + x + 1]);

                hor = (hor<0)?(-hor):hor;
                ver = (ver<0)?(-ver):ver;
                imgOut[y*width+x] = (int) Math.ceil(((hor + ver) >> 1)*1.41);
            }
        }
        return imgOut;
    }
}
