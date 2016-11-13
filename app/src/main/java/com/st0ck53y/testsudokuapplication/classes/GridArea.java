package com.st0ck53y.testsudokuapplication.classes;

public class GridArea {
    private int value;

    public GridArea(int v) {
        this();
        if (v != 0) {
            setValue(v);
        }
    }

    public GridArea() {
        value = 8185; // 4 bit for remaining possible (9), 9 bit for flags of each, and 1 bit for value flag (is value or check?)
    }

    public boolean isValue() {
        return (value >> 15) == 1; //value flag set when number is certain
    }

    public int getValue() {
        return (value & 15); //either the number in grid, or possibilities remaining
    }

    //creates list of integers from binary flags
    public int[] getPossibles() {
        int[] p = new int[getValue()];
        int pointer = 0;
        for (int i = 4; i <= 12; i++) {
            if (((value >> i) & 1) == 1) {
                p[pointer++] = i - 3;
            }
        }
        return p;
    }

    //switches from check to value
    public void checkGrid() {
        if ((getValue()) == 1) {
            setValue(getPossibles()[0]);
        }
    }

    //turns off flag of number and decrements remaining counter
    public void setImpossible(int i) {
        int pos = i + 3;
        int curSet = (value >> pos) & 1;
        if (curSet == 1) { //Have to check to stop decrement if multiple passes are done
            value &= (65535 - (1 << (i + 3)));
            value--;
            checkGrid();
        }
    }

    public void setValue(int v) {
        if (v == 0)
            value = 8185;
        else
            value = (v | (1 << 15));
    }
}
