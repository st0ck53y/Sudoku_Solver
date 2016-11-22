package com.st0ck53y.testsudokuapplication.classes;

public class Canny {
    private int[] image;
    private int[] gradient;
    private int[] direction;
    private int w, h;
    private int tH, tL;

    public Canny(int[] imgIn, int width, int height, int blurDiameter, int edgeThresholdHigh, int edgeThresholdLow) {
        this.image = ImageProcessor.gaussianBlur(imgIn, width, height, blurDiameter);
        this.gradient = new int[width*height];
        this.direction = new int[width*height];
        this.w = width;
        this.h = height;
        this.tH = edgeThresholdHigh;
        this.tL = edgeThresholdLow;
    }

    public int[] getImage() {
        return image;
    }

    public int[] getGradient() {
        return gradient;
    }

    public void computeGradientAngles() {
        for (int y = 0; y < h - 1; y++) {
            int yOffs = y * w;
            int yPos  = yOffs + w;
            for (int x = 0; x < w - 1; x++) {
                double xd = computeXDerivative(image[yOffs + x], image[yOffs + x + 1], image[yPos + x], image[yPos + x + 1]);
                double yd = computeYDerivative(image[yOffs + x], image[yOffs + x + 1], image[yPos + x], image[yPos + x + 1]);
                gradient[yOffs+x] = computeGradient(xd,yd);
                direction[yOffs+x] = computeDirection(xd,yd);
            }
        }
    }

    public void suppressNonMaxima() {
        for (int y = 0; y < h - 1; y++) {
            int yOffs = y * w;
            for (int x = 0; x < w - 1; x++) {
                if (y == 0 || x == 0) {
                    image[yOffs+x] = 0;
                    continue;
                }
                int dir = direction[yOffs+x];
                if (dir < -67 || dir > 67) { //Horizontal
                    if (gradient[yOffs+x] < gradient[yOffs+x+w] || gradient[yOffs+x] < gradient[yOffs+x-w]) {
                        image[yOffs+x] = 0;
                    } else {
                        image[yOffs+x] = gradient[yOffs+x];
                    }
                } else if (dir > -23 && dir < 23) { //Vertical
                    if (gradient[yOffs+x] < gradient[yOffs+x+1] || gradient[yOffs+x] < gradient[yOffs+x-1]) {
                        image[yOffs+x] = 0;
                    } else {
                        image[yOffs+x] = gradient[yOffs+x];
                    }
                } else if (dir < -22) { //down right - up left
                    if (gradient[yOffs+x] < gradient[yOffs+x+(w-1)] || gradient[yOffs+x] < gradient[yOffs+x-(w-1)]) {
                        image[yOffs+x] = 0;
                    } else {
                        image[yOffs+x] = gradient[yOffs+x];
                    }
                } else { //up right - down left
                    if (gradient[yOffs+x] < gradient[yOffs+x+(w+1)] || gradient[yOffs+x] < gradient[yOffs+x-(w+1)]) {
                        image[yOffs+x] = 0;
                    } else {
                        image[yOffs+x] = gradient[yOffs+x];
                    }
                }
            }
        }
    }

    public void applyThresholds() {
        int[] lower = new int[w*h];
        int[] higher = new int[w*h];
        int[] visited = new int[w*h];
        int[] promote = new int[w*h];
        for (int i = 0; i < w*h; i++) {
            if (image[i] > tH) {
                higher[i] = image[i];
            } else if (image[i] > tL) {
                lower[i] = image[i];
            }
        }

        //hysteresis
    }

    private static double computeXDerivative(int... imgKernel) {
        return ((imgKernel[1] - imgKernel[0]) + (imgKernel[3] - imgKernel[2]))/2;
    }

    private static double computeYDerivative(int... imgKernel) {
        return ((imgKernel[2] - imgKernel[0]) + (imgKernel[3] - imgKernel[1]))/2;
    }

    private static int computeGradient(double x, double y) {
        return (int) Math.round(Math.sqrt((x*x)+(y*y)));
    }

    private static int computeDirection(double x, double y) {
        return (int) Math.round(Math.toDegrees(Math.atan(y/x)));
    }

}
