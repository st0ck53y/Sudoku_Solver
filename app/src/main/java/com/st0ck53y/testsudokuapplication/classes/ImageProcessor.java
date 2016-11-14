package com.st0ck53y.testsudokuapplication.classes;

public class ImageProcessor {

    /**
     * Blurs a single channel image using gaussian method with kernel size of 3
     *
     * @param imgIn array of pixel data for a single channel
     * @param width width of image
     * @param height height of image
     * @return single channel image blurred
     */
    public static int[] gaussianBlur(int[] imgIn, int width, int height) {
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

                imgOut[y*width+x] = (imgIn[yNeg + x - 1] >> 4) +
                        (imgIn[yNeg + x] >> 3) +
                        (imgIn[yNeg + x + 1] >> 4) +
                        (imgIn[yOffs + x - 1] >> 3) +
                        (imgIn[yOffs + x] >> 2) +
                        (imgIn[yOffs + x + 1] >> 3) +
                        (imgIn[yPos + x - 1] >> 4) +
                        (imgIn[yPos + x] >> 3) +
                        (imgIn[yPos + x + 1] >> 4);
            }
        }
        return imgOut;
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
