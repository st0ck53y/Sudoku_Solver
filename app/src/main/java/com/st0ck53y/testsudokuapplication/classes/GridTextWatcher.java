package com.st0ck53y.testsudokuapplication.classes;

import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;

public class GridTextWatcher implements TextWatcher {
    private EditText e;
    private Button solve;
    private Context context;
    private boolean ch;
    private String cs;
    public GridTextWatcher(EditText e, Button s, Context context) {
        this.e = e;
        this.solve = s;
        this.context = context;
    }

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        if (1==2 /* Check user input vs Solution input */) //TODO
            return;
        if (s.length() != 0) {
            cs = s.toString();
            ch = true;
        }
    }

    @Override
    public void onTextChanged(CharSequence s, int start, int before, int count) {
        if (s.length() != 0) {
            ((InputMethodManager)context.getSystemService(Context.INPUT_METHOD_SERVICE))
                    .hideSoftInputFromWindow(solve.getWindowToken(), 0);
            e.clearFocus();
            ((RelativeLayout) (e.getParent()).getParent()).requestFocus();
        }
    }

    @Override
    public void afterTextChanged(Editable s) {
        if (ch) {
            ch = false;
            String text;
            if (s.charAt(0) == cs.charAt(0)) {
                text = Character.toString(s.charAt(1));
            } else {
                text = Character.toString(s.charAt(0));
            }
            int i = Integer.valueOf(text);
            if (i == 0) text = "";
            Log.i("tChange", text);
            e.removeTextChangedListener(this);
            e.setText(text);
            e.addTextChangedListener(this);
        }
    }
}
