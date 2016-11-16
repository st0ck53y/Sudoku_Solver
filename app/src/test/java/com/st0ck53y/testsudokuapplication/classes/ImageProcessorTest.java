package com.st0ck53y.testsudokuapplication.classes;

import org.junit.Test;

import static org.hamcrest.CoreMatchers.equalTo;
import static org.junit.Assert.*;

public class ImageProcessorTest {

    @Test
    public void whiteBlursToWhite() throws Exception {
        int[] allWhite = new int[] {255, 255, 255, 255, 255, 255, 255, 255, 255};
        int blurredPixel = ImageProcessor.blurPixel3(allWhite, 1, 3, 0, 6);
        assertThat(blurredPixel, equalTo(255));
    }

    @Test
    public void blackBlursToBlack() throws Exception {
        int[] allWhite = new int[] {0, 0, 0, 0, 0, 0, 0, 0, 0};
        int blurredPixel = ImageProcessor.blurPixel3(allWhite, 1, 3, 0, 6);
        assertThat(blurredPixel, equalTo(0));
    }

    @Test
    public void cGreyBlursToCGrey() throws Exception {
        int[] allWhite = new int[] {127, 127, 127, 127, 127, 127, 127, 127, 127};
        int blurredPixel = ImageProcessor.blurPixel3(allWhite, 1, 3, 0, 6);
        assertThat(blurredPixel, equalTo(127));
    }

    @Test
    public void bgwBlursToBgw() throws Exception {
        int[] allWhite = new int[] {0, 0, 0, 127, 127, 127, 255, 255, 255};
        int blurredPixel = ImageProcessor.blurPixel3(allWhite, 1, 3, 0, 6);
        assertThat(blurredPixel, equalTo(127));
    }
}