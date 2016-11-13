package com.st0ck53y.testsudokuapplication.data;

import android.os.AsyncTask;

import com.st0ck53y.testsudokuapplication.activity.NewGrid;
import com.st0ck53y.testsudokuapplication.classes.Solver;

public class SolverTask extends AsyncTask<String, Void, int[][]> {

    private int[][] grid;
    private NewGrid gr;

    public SolverTask(int[][] g, NewGrid gr) {
        grid = g;
        this.gr = gr;
    }

    @Override
    protected int[][] doInBackground(String... values) {
        Solver s = new Solver(grid);
        s.solvePuzzle();
        return s.getSolution(0);
    }

    @Override
    protected void onPreExecute() {
        gr.processing = true;
        gr.solveText("Solving");
    }

    @Override
    protected void onPostExecute(int[][] result) {
        gr.processing = false;
        NewGrid.solveButt = false;
        if (result[0][0]==0) {
            gr.solveText("Imposibru");
        } else {
            gr.solveText("Clear");
            gr.setGridValues(result);
        }
    }
}
