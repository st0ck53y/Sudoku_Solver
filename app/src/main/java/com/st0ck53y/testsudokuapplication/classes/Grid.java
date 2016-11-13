package com.st0ck53y.testsudokuapplication.classes;


public class Grid {

    public GridArea[][] grid;
    public boolean solved;

    public Grid(int[][] start) {
        solved = false;
        grid = new GridArea[9][9];
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                grid[i][j] = new GridArea(start[i][j]);
            }
        }
    }
}
