#pragma once

#include <vector>
#include <cmath>
#include "grishagin_function.h"

using namespace std;

double grishaginFunction(int problemIndex, const vector<double>& x) {
    if (x.size() != 2) return 0.0;

    TGrishaginProblem problem(problemIndex);
    return problem.Compute(0, x);
}

void getGrishaginBounds(int problemIndex, vector<double>& lower, vector<double>& upper) {
    lower = { 0.0, 0.0 };
    upper = { 1.0, 1.0 };
}

vector<double> getGrishaginOptimum(int problemIndex, double& minValue) {
    vector<double> optimum = {
        rand_minimums[2 * (problemIndex - 1)],
        rand_minimums[2 * (problemIndex - 1) + 1]
    };

    TGrishaginProblem problem(problemIndex);
    minValue = problem.Compute(0, optimum);

    return optimum;
}

const int NUM_GRISHAGIN_TESTS = 100;