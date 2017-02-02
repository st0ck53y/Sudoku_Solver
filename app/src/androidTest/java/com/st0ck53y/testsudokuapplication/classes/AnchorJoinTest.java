package com.st0ck53y.testsudokuapplication.classes;

import android.support.test.rule.ActivityTestRule;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import com.st0ck53y.testsudokuapplication.activity.Home;
import com.st0ck53y.testsudokuapplication.helper.NativeHelper;

import org.junit.Test;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

public class AnchorJoinTest extends ActivityTestRule<Home> {

    public AnchorJoinTest(){
        super(Home.class);
    }

    @Test
    public void simpleTest(){
        NativeHelper.init();
        int w = 0;
        int h = 0;
        byte[] img = {};
        int[] expectedImg = {};
        int[] actualImg = new int[w*h];
        int[] anc = {};
        IntBuffer out = ByteBuffer.allocateDirect(w*h*4).order(ByteOrder.LITTLE_ENDIAN).asIntBuffer();

        NativeHelper.joinAnchors(img,w,h,NativeHelper.tanned,anc,out);
        out.get(actualImg).rewind();
        out.position(0);
        assertArrayEquals(expectedImg,actualImg);
    }
}
