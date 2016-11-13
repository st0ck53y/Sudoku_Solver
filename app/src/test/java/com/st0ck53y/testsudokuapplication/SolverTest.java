package com.st0ck53y.testsudokuapplication;

import com.st0ck53y.testsudokuapplication.classes.Solver;

import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;

public class SolverTest {

    /*

    Solver testing template

        int[][] startGrid = {{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}};
        int[][] expctedSol= {{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0}};
        Solver s = new Solver(startGrid);
        s.solvePuzzle();
        int[][] actualSol = s.getSolution();
        for (int i = 0; i < 9; i++) {
            assertArrayEquals(expectedSol[i], actualSol[i]);
        }


     */


    @Test
    public void solve1() throws Exception {
        int[][] startGrid = {{5,3,0,6,0,0,0,9,8},{0,7,0,1,9,5,0,0,0},{0,0,0,0,0,0,0,6,0},
                             {8,0,0,4,0,0,7,0,0},{0,6,0,8,0,3,0,2,0},{0,0,3,0,0,1,0,0,6},
                             {0,6,0,0,0,0,0,0,0},{0,0,0,4,1,9,0,8,0},{2,8,0,0,0,5,0,7,9}};
        int[][] expectedSol={{5,3,4,6,7,2,1,9,8},{6,7,8,1,9,5,3,4,2},{9,1,2,3,4,8,5,6,7},
                             {8,5,9,4,2,6,7,1,3},{7,6,1,8,5,3,9,2,4},{4,2,3,7,9,1,8,5,6},
                             {9,6,1,2,8,7,3,4,5},{5,3,7,4,1,9,2,8,6},{2,8,4,6,3,5,1,7,9}};
        Solver s = new Solver(startGrid);
        s.solvePuzzle();
        int[][] actualSol = s.getSolution();
        for (int i = 0; i < 9; i++) {
            assertArrayEquals(expectedSol[i], actualSol[i]);
        }
    }
}