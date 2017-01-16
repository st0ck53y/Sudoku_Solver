package com.st0ck53y.testsudokuapplication.activity;

import android.content.Intent;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.st0ck53y.testsudokuapplication.R;
import com.st0ck53y.testsudokuapplication.classes.CameraView;

public class ScanGrid extends AppCompatActivity {

    private Camera cam = null;
    private CameraView cv = null;
    public static ImageView iv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_scan_grid);

        iv = (ImageView) findViewById(R.id.image_view);
        iv.setAlpha(1f);

        try {
            cam = Camera.open();
        } catch (Exception e) {
            Log.w("Camera", "Could not access Camera");
        }

        if (cam != null && cv == null) {
            cv = new CameraView(this, cam);
            FrameLayout camera_view = (FrameLayout) findViewById(R.id.camera_view);
            camera_view.addView(cv);
        }
    }

    @Override
    protected void onResume() {
        Log.i("scan","resume");
        if (cam == null) {
            try {
                cam = Camera.open();
                if (cv == null) {
                    cv = new CameraView(this, cam);
                    FrameLayout camera_view = (FrameLayout) findViewById(R.id.camera_view);
                    camera_view.addView(cv);
                }
            } catch (Exception e) {
                Log.e("err",e.toString());
                Intent i = new Intent();
                i.setClass(getApplicationContext(), Home.class);
                i.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(i);
            }
        }
        super.onResume();
    }
    @Override
    protected void onPause() {
        Log.i("scan","pause");
        if (cam != null) {
            cam.stopPreview();
            cam.setPreviewCallback(null);
            cam.lock();
            cam.release();
            cam=null;
            cv = null;
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        super.onPause();
    }
    @Override
    protected void onDestroy() {
        if (cam != null) {
            cam.release();
            cam = null;
        }
        super.onDestroy();
    }
}
