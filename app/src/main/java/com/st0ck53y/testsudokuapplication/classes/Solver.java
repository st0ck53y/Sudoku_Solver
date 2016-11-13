package com.st0ck53y.testsudokuapplication.classes;

public class Solver {

    private Grid grid;

    public Solver(int[][] grid) {
        this.grid = new Grid(grid);
    }

    public void solvePuzzle() {
        boolean unsolved;
        do {
            unsolved = false;
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (grid.grid[i][j].isValue()) {
                        int curVal = grid.grid[i][j].getValue();
                        for (int jSet = 0; jSet < 9; jSet++) {
                            if (!grid.grid[i][jSet].isValue()) {
                                grid.grid[i][jSet].setImpossible(curVal);
                            }
                        }
                        int[] iVals = new int[4];
                        switch (i % 3) {
                            case 0:
                                iVals[0] = 1;
                                iVals[1] = 2;
                                break;
                            case 1:
                                iVals[0] = -1;
                                iVals[1] = 1;
                                break;
                            case 2:
                                iVals[0] = -2;
                                iVals[1] = -1;
                                break;
                        }
                        if (i >= 6) {
                            iVals[2] = -6;
                            iVals[3] = -3;
                        } else if (i >= 3) {
                            iVals[2] = -3;
                            iVals[3] = 3;
                        } else {
                            iVals[2] = 3;
                            iVals[3] = 6;
                        }
                        int jVal = 0;
                        if (j >= 6)
                            jVal = 6;
                        else if (j >= 3)
                            jVal = 3;
                        for (int y = 0; y < 3; y++) {
                            grid.grid[i + iVals[0]][jVal + y].setImpossible(curVal);
                            grid.grid[i + iVals[1]][jVal + y].setImpossible(curVal);
                        }
                        jVal = (j%3);
                        for (int x = 0; x < 3; x++) {
                            grid.grid[i + iVals[2]][jVal + (3*x)].setImpossible(curVal);
                            grid.grid[i + iVals[3]][jVal + (3*x)].setImpossible(curVal);
                        }
                    } else {
                        unsolved = true;
                    }
                }
            }
        } while (unsolved);
    }

    public int[][] getSolution(int nullit) {
        int[][] solution = new int[9][9];
        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 9; y++) {
                solution[x][y] = grid.grid[x][y].getValue();
            }
        }
        return solution;
    }

    public Grid getSolution() {
        return grid;
    }
}
