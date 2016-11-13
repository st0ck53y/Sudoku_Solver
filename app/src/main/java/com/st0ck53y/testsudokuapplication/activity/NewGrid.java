package com.st0ck53y.testsudokuapplication.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;

import com.st0ck53y.testsudokuapplication.R;
import com.st0ck53y.testsudokuapplication.classes.GridTextWatcher;

public class NewGrid extends AppCompatActivity {

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_new_grid);
        Button solve = (Button) findViewById(R.id.butSolve);
        EditText[][] grid = new EditText[9][9];
        grid[0][0] = (EditText) findViewById(R.id._11);
        ((RelativeLayout) (grid[0][0].getParent()).getParent()).requestFocus();
        grid[0][1] = (EditText) findViewById(R.id._12);
        grid[0][2] = (EditText) findViewById(R.id._13);
        grid[0][3] = (EditText) findViewById(R.id._14);
        grid[0][4] = (EditText) findViewById(R.id._15);
        grid[0][5] = (EditText) findViewById(R.id._16);
        grid[0][6] = (EditText) findViewById(R.id._17);
        grid[0][7] = (EditText) findViewById(R.id._18);
        grid[0][8] = (EditText) findViewById(R.id._19);
        grid[1][0] = (EditText) findViewById(R.id._21);
        grid[1][1] = (EditText) findViewById(R.id._22);
        grid[1][2] = (EditText) findViewById(R.id._23);
        grid[1][3] = (EditText) findViewById(R.id._24);
        grid[1][4] = (EditText) findViewById(R.id._25);
        grid[1][5] = (EditText) findViewById(R.id._26);
        grid[1][6] = (EditText) findViewById(R.id._27);
        grid[1][7] = (EditText) findViewById(R.id._28);
        grid[1][8] = (EditText) findViewById(R.id._29);
        grid[2][0] = (EditText) findViewById(R.id._31);
        grid[2][1] = (EditText) findViewById(R.id._32);
        grid[2][2] = (EditText) findViewById(R.id._33);
        grid[2][3] = (EditText) findViewById(R.id._34);
        grid[2][4] = (EditText) findViewById(R.id._35);
        grid[2][5] = (EditText) findViewById(R.id._36);
        grid[2][6] = (EditText) findViewById(R.id._37);
        grid[2][7] = (EditText) findViewById(R.id._38);
        grid[2][8] = (EditText) findViewById(R.id._39);
        grid[3][0] = (EditText) findViewById(R.id._41);
        grid[3][1] = (EditText) findViewById(R.id._42);
        grid[3][2] = (EditText) findViewById(R.id._43);
        grid[3][3] = (EditText) findViewById(R.id._44);
        grid[3][4] = (EditText) findViewById(R.id._45);
        grid[3][5] = (EditText) findViewById(R.id._46);
        grid[3][6] = (EditText) findViewById(R.id._47);
        grid[3][7] = (EditText) findViewById(R.id._48);
        grid[3][8] = (EditText) findViewById(R.id._49);
        grid[4][0] = (EditText) findViewById(R.id._51);
        grid[4][1] = (EditText) findViewById(R.id._52);
        grid[4][2] = (EditText) findViewById(R.id._53);
        grid[4][3] = (EditText) findViewById(R.id._54);
        grid[4][4] = (EditText) findViewById(R.id._55);
        grid[4][5] = (EditText) findViewById(R.id._56);
        grid[4][6] = (EditText) findViewById(R.id._57);
        grid[4][7] = (EditText) findViewById(R.id._58);
        grid[4][8] = (EditText) findViewById(R.id._59);
        grid[5][0] = (EditText) findViewById(R.id._61);
        grid[5][1] = (EditText) findViewById(R.id._62);
        grid[5][2] = (EditText) findViewById(R.id._63);
        grid[5][3] = (EditText) findViewById(R.id._64);
        grid[5][4] = (EditText) findViewById(R.id._65);
        grid[5][5] = (EditText) findViewById(R.id._66);
        grid[5][6] = (EditText) findViewById(R.id._67);
        grid[5][7] = (EditText) findViewById(R.id._68);
        grid[5][8] = (EditText) findViewById(R.id._69);
        grid[6][0] = (EditText) findViewById(R.id._71);
        grid[6][1] = (EditText) findViewById(R.id._72);
        grid[6][2] = (EditText) findViewById(R.id._73);
        grid[6][3] = (EditText) findViewById(R.id._74);
        grid[6][4] = (EditText) findViewById(R.id._75);
        grid[6][5] = (EditText) findViewById(R.id._76);
        grid[6][6] = (EditText) findViewById(R.id._77);
        grid[6][7] = (EditText) findViewById(R.id._78);
        grid[6][8] = (EditText) findViewById(R.id._79);
        grid[7][0] = (EditText) findViewById(R.id._81);
        grid[7][1] = (EditText) findViewById(R.id._82);
        grid[7][2] = (EditText) findViewById(R.id._83);
        grid[7][3] = (EditText) findViewById(R.id._84);
        grid[7][4] = (EditText) findViewById(R.id._85);
        grid[7][5] = (EditText) findViewById(R.id._86);
        grid[7][6] = (EditText) findViewById(R.id._87);
        grid[7][7] = (EditText) findViewById(R.id._88);
        grid[7][8] = (EditText) findViewById(R.id._89);
        grid[8][0] = (EditText) findViewById(R.id._91);
        grid[8][1] = (EditText) findViewById(R.id._92);
        grid[8][2] = (EditText) findViewById(R.id._93);
        grid[8][3] = (EditText) findViewById(R.id._94);
        grid[8][4] = (EditText) findViewById(R.id._95);
        grid[8][5] = (EditText) findViewById(R.id._96);
        grid[8][6] = (EditText) findViewById(R.id._97);
        grid[8][7] = (EditText) findViewById(R.id._98);
        grid[8][8] = (EditText) findViewById(R.id._99);
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                Log.i("tst",""+i+","+j);
                grid[i][j].addTextChangedListener(new GridTextWatcher(grid[i][j], solve, this));
            }
        }
    }
}
