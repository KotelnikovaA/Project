#pragma once

#include <vector>
#include <cmath>
#include "GKLSProblem.h"

using namespace std;

double gklsFunction(int problemIndex, const vector<double>& x,
    int dim = 2, GKLSClass type = Simple,
    GKLSFuncionType functionType = TD) {
    if ((int)x.size() != dim) return 1e100;
    TGKLSProblem problem(problemIndex, dim, type, functionType);
    return problem.Compute(0, x);
}

void getGKLSBounds(int dim, vector<double>& lower, vector<double>& upper) {
    lower.assign(dim, -1.0);
    upper.assign(dim, 1.0);
}

double getGKLSMinimumValue(int problemIndex, int dim = 2,
    GKLSClass type = Simple, GKLSFuncionType functionType = TD) {
    TGKLSProblem problem(problemIndex, dim, type, functionType);
    return problem.getGlobalMinimumValue();
}

struct GKLSGlobalMinimaInfo {
    double value;
    vector<vector<double>> points;
};

GKLSGlobalMinimaInfo getGKLSGlobalMinima(int problemIndex, int dim = 2,
    GKLSClass type = Simple,
    GKLSFuncionType functionType = TD) {
    GKLSGlobalMinimaInfo info;
    TGKLSProblem problem(problemIndex, dim, type, functionType);

    info.value = problem.getGlobalMinimumValue();
    int numMinima = problem.getNumGlobalMinima();

    for (int i = 0; i < numMinima; i++) {
        info.points.push_back(problem.getGlobalMinimumPoint(i));
    }

    return info;
}

bool isGKLSGlobalMinimum(int problemIndex, const vector<double>& point,
    double value, int dim = 2,
    GKLSClass type = Simple,
    GKLSFuncionType functionType = TD,
    double tolerance = 1e-6) {
    TGKLSProblem problem(problemIndex, dim, type, functionType);
    return problem.isGlobalMinimumPoint(point, tolerance);
}

const int NUM_GKLS_TESTS = 100;