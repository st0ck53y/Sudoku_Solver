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
        int a = w*h;
        int[] lower = new int[a];
        int[] higher = new int[a];
        boolean[] visited = new boolean[a];
        for (int i = 0; i < a; i++) {
            if (image[i] > tH) {
                higher[i] = image[i];
                lower[i] = image[i]; //put in lower too because the line crawling is simpler as a single variable to check
            } else if (image[i] > tL) {
                lower[i] = image[i];
            } else {
                image[i] = 0;
            }
        }

        int cy,cx;
        boolean np;
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
                        image[(cy*w)+cx] = lower[(cy*w)+cx];
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

        for (int i = 0; i < a; i++) {
            if (image[i] != 0) image[i] = 255;
        }
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

    private static int[] findPixel(int last, int[] grid) {

        return new int[]{};
    }
}
