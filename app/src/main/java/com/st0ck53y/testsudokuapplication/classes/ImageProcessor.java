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
     * @return single channel image blurred
     */
    public static int[] gaussianBlurNS3(int[] imgIn, int width, int height) {
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
                imgOut[y*width+x] = blurPixel(imgIn, x, yOffs, yNeg, yPos);
            }
        }
        return imgOut;
    }

    static int blurPixel(int[] imgIn, int x, int row, int prevRow, int nextRow) {
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
