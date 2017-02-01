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
import com.st0ck53y.testsudokuapplication.helper.NativeHelper;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.util.List;

public class CameraView extends SurfaceView implements SurfaceHolder.Callback,Camera.PreviewCallback {
    private boolean processing = false;
    private int imageFormat;

    Matrix matrix;
    private Bitmap scaled = null;
    private Bitmap rotate = null;
    private Bitmap bitmap = null;
    IntBuffer out;
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

        matrix = new Matrix();
        matrix.postRotate(90);

        bitmap = Bitmap.createBitmap(PreviewSizeWidth, PreviewSizeHeight, Bitmap.Config.ARGB_8888);
        pixels = new int[PreviewSizeWidth * PreviewSizeHeight];
        //must specify endianness.... native is big endian, this is not :'(
        this.out = ByteBuffer.allocateDirect(PreviewSizeWidth*PreviewSizeHeight*4).order(ByteOrder.LITTLE_ENDIAN).asIntBuffer();
    }

    int frame = 0;
    @Override
    public void onPreviewFrame(byte[] arg0, Camera cam) {
        if (frame < 60) {
            frame++;
            return;
        }
        if (imageFormat == ImageFormat.NV21) {
            if (!processing) {
                processing = true;
                ScanGrid.iv.setImageBitmap(rotate);
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

    static long totalTime = 0;
    static long thisTime = 0;
    static long times = 0;

    Runnable processFrame = new Runnable() {
        @Override
        public void run() {
            processing = true;
            long ts = System.nanoTime();
            int w = PreviewSizeWidth;
            int h = PreviewSizeHeight;

            NativeHelper.nativeCanny(framedat,w,h,NativeHelper.tanned,15,35,out);
            out.get(pixels).rewind();
            out.position(0);

            long te = System.nanoTime();
            thisTime = (te-ts)/1000000;
            totalTime+=thisTime;
            times++;
            Log.i("avg times", ""+totalTime/times);
            bitmap.setPixels(pixels, 0, w, 0, 0, w, h);

            scaled = Bitmap.createScaledBitmap(bitmap,PreviewSizeWidth,PreviewSizeHeight,true);
            rotate = Bitmap.createBitmap(scaled , 0, 0, scaled.getWidth(), scaled.getHeight(), matrix, true);

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
        } catch (Exception e) {
            //
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
        } catch (Exception e) {
            //
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        try {
            camera.stopPreview();
            camera.release();
        } catch (Exception e) {
            //
        }
    }
}
