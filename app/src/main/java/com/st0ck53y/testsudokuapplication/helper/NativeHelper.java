package com.st0ck53y.testsudokuapplication.helper;

import java.io.Serializable;
import java.nio.IntBuffer;

public class NativeHelper implements Serializable {

    public static int[] tanned;

    public static void init()  {
        System.loadLibrary("ImageProcessing");
        tanned = new int[1<<18];
        for (int y = -255; y < 256; y++) {
            for (int x = -255; x < 256; x++) {
                int yIdx = (y & 0xff) | (y<0?1<<8:0);
                int xIdx = (x & 0xff) | (x<0?1<<8:0);
                if (x==0) {
                    if (y==0){
                        tanned[0] = 90;
                    } else {
                        tanned[yIdx<<9] = 0;
                    }
                }else {
                    tanned[yIdx<<9|xIdx&0x1ff] = (int) Math.round(Math.toDegrees(Math.atan(y/x)));
                }
            }
        }
    }

    public native static void nativeCanny(byte[] imgData, int width, int height, int[] preDirs, int lower, int higher, IntBuffer imgOut);

}
