#pragma once

#include <vector>
#include <cmath>
#include "ShekelProblem.h"

using namespace std;

double shekelFunction(int problemIndex, const vector<double>& x) {
    if (x.size() != 1) return 0.0;

    TShekelProblem problem(problemIndex);
    return problem.Compute(problemIndex, x);
}

void getShekelBounds(int problemIndex, vector<double>& lower, vector<double>& upper) {
    lower = { 0.0 };
    upper = { 10.0 };
}

vector<double> getShekelOptimum(int problemIndex, double& minValue) {
    minValue = minShekel[problemIndex][0];
    return { minShekel[problemIndex][1] };
}

const int NUM_SHEKEL_TESTS = NUM_SHEKEL_PROBLEMS;