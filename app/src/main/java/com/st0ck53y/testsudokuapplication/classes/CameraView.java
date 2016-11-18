package com.st0ck53y.testsudokuapplication.classes;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.hardware.Camera;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.st0ck53y.testsudokuapplication.activity.ScanGrid;

import java.io.IOException;
import java.util.List;

public class CameraView extends SurfaceView implements SurfaceHolder.Callback,Camera.PreviewCallback {
    private boolean processing = false;
    private int imageFormat;

    private Bitmap bitmap = null;
    private int[] pixels = null;
    private byte[] framedat = null;

    private SurfaceHolder sHolder;
    private Camera camera;


    Handler handler = new Handler(Looper.getMainLooper());

    static int PreviewSizeWidth;
    static int PreviewSizeHeight;


    public CameraView(Context context, Camera c) {
        super(context);

        camera = c;
        camera.setDisplayOrientation(90);
        Camera.Parameters cPar = c.getParameters();
        cPar.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        c.setParameters(cPar);

        sHolder = getHolder();
        sHolder.addCallback(this);
        sHolder.setType(SurfaceHolder.SURFACE_TYPE_NORMAL);

        List<Camera.Size> sizes = cPar.getSupportedPreviewSizes();
        PreviewSizeWidth = sizes.get(0).width;
        PreviewSizeHeight = sizes.get(0).height;

        bitmap = Bitmap.createBitmap(PreviewSizeWidth, PreviewSizeHeight, Bitmap.Config.ARGB_8888);
        pixels = new int[PreviewSizeWidth * PreviewSizeHeight];
    }

    int frame = 0;
    @Override
    public void onPreviewFrame(byte[] arg0, Camera cam) {
        if (frame < 5) {
            frame++;
            return;
        }
        if (imageFormat == ImageFormat.NV21) {
            if (!processing) {
                framedat = arg0;
                handler.post(processFrame);
            }
        }
    }

    /**
     * taken from http://stackoverflow.com/questions/12469730/confusion-on-yuv-nv21-conversion-to-rgb
     * because who knows how this works... ill figure it out at some point....
     */
    public static void YUV_NV21_TO_RGB(int[] argb, byte[] yuv, int width, int height) {
        final int frameSize = width * height;

        final int ii = 0;
        final int ij = 0;
        final int di = +1;
        final int dj = +1;

        int a = 0;
        for (int i = 0, ci = ii; i < height; ++i, ci += di) {
            for (int j = 0, cj = ij; j < width; ++j, cj += dj) {
                int y = (0xff & ((int) yuv[ci * width + cj]));
                int v = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 0]));
                int u = (0xff & ((int) yuv[frameSize + (ci >> 1) * width + (cj & ~1) + 1]));
                y = y < 16 ? 16 : y;

                int r = (int) (1.164f * (y - 16) + 1.596f * (v - 128));
                int g = (int) (1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
                int b = (int) (1.164f * (y - 16) + 2.018f * (u - 128));

                r = r < 0 ? 0 : (r > 255 ? 255 : r);
                g = g < 0 ? 0 : (g > 255 ? 255 : g);
                b = b < 0 ? 0 : (b > 255 ? 255 : b);

                argb[a++] = 0xff000000 | (r << 16) | (g << 8) | b;
            }
        }
    }

    private int[] yFromYUV() {
        int[] dat = new int[PreviewSizeWidth * PreviewSizeHeight];
        int datLow[] = new int[2];
        int datHigh[] = new int[2];
        for (int i = 0; i < dat.length; i++) {
            dat[i] = framedat[i]&0xff; //GODDANGIT!! its not unsigned!!
//            dat[i] += 128; // this seems to screw it up, even though its the same?
            if (framedat[i] < datLow[0]) datLow[0] = framedat[i];
            if (framedat[i] >datHigh[0]) datHigh[0]= framedat[i];
            if (     dat[i] < datLow[1]) datLow[1] =      dat[i];
            if (     dat[i] >datHigh[1]) datHigh[1]=      dat[i];
        }
        return dat;
    }

    private void lumToRGB(int[] lum) {
        for (int i = 0; i < lum.length; i++) {
            pixels[i] = 0xff000000 | ((lum[i] & 0xff) << 16) | ((lum[i] & 0xff) << 8) | (lum[i] & 0xff);
        }
    }

    private void sobelEdgeDetectorPoor(int[] pxl) {
        int w, h;
        w = PreviewSizeWidth;
        h = PreviewSizeHeight;
        int[] diffed = ImageProcessor.sobelDifferential(pxl,w,h);
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int val = diffed[y*w+x];
                if (val < 30) val = 0;
                else if (val >= 30) val = 0xff;
                pixels[(y*w) + x] = 0xff000000 | ((val & 0xff) << 16) | ((val & 0xff) << 8) | (val & 0xff);
            }
        }
        //takes ~ 1.5 seconds on average
    }



    static long totalTime = 0;
    static long thisTime = 0;
    static long times = 0;

    Runnable processFrame = new Runnable() {
        @Override
        public void run() {
            processing = true;
//            YUV_NV21_TO_RGB(pixels,framedat,PreviewSizeWidth,PreviewSizeHeight);
            int[] pixLum = yFromYUV();
            long ts = System.nanoTime();
            pixLum = ImageProcessor.gaussianBlur(pixLum,PreviewSizeWidth,PreviewSizeHeight, 7);
//            sobelEdgeDetectorPoor(pixLum);
            long te = System.nanoTime();
            thisTime = (te-ts)/1000000;
            totalTime+=thisTime;
            times++;
            Log.i("avg times", ""+totalTime/times);
            lumToRGB(pixLum);
            bitmap.setPixels(pixels, 0, PreviewSizeWidth, 0, 0, PreviewSizeWidth, PreviewSizeHeight);
//
            Matrix matrix = new Matrix();
            matrix.postRotate(90);
            Bitmap scaledBitmap = Bitmap.createScaledBitmap(bitmap,PreviewSizeWidth,PreviewSizeHeight,true);
            Bitmap rotatedBitmap = Bitmap.createBitmap(scaledBitmap , 0, 0, scaledBitmap .getWidth(), scaledBitmap .getHeight(), matrix, true);

            ScanGrid.iv.setImageBitmap(rotatedBitmap);
            processing = false;
        }
    };

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        try{
            Camera.Parameters p = camera.getParameters();
            List<Integer> formats = p.getSupportedPictureFormats();
            imageFormat = p.getPreviewFormat();
            camera.setPreviewDisplay(surfaceHolder);
            camera.startPreview();
        } catch (IOException ex) {
            Log.w("Camera","Preview could not be displayed, surfaceCreated Err");
        }

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i1, int i2, int i3){
        if (sHolder.getSurface() == null) return;
        try {
            camera.stopPreview();
        } catch (Exception e) {

        }

        try {
            camera.setPreviewDisplay(sHolder);
            camera.setPreviewCallback(this);
            camera.startPreview();
        }catch (IOException ex) {
            Log.w("Camera","Preview could not be displayed, surfaceChanged Err");
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        camera.stopPreview();
        camera.release();
    }
}
