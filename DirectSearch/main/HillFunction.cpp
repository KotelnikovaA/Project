#include "HillFunction.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

double hillFunction(int problemIndex, const vector<double>& x) {
    double t = x[0];
    double result = 0.0;

    for (int i = 0; i < NUM_HILL_COEFF; i++) {
        double angle = 2.0 * i * M_PI * t;
        result += aHill[problemIndex][i] * sin(angle)
            + bHill[problemIndex][i] * cos(angle);
    }

    return result;
}

void getHillBounds(vector<double>& lower, vector<double>& upper) {
    lower = {0.0};
    upper = {1.0};
}

vector<double> getHillOptimum(int problemIndex, double& optimumValue) {
    optimumValue = minHill[problemIndex][0];
    return { minHill[problemIndex][1] };
}