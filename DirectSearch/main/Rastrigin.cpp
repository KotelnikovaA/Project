#include "Rastrigin.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

double rastrigin(const vector<double>& x) {
    double sum = 0.0;
    for (double xi : x) {
        sum += xi * xi - 10.0 * cos(2.0 * M_PI * xi) + 10.0;
    }
    return sum;
}

void getRastriginBounds(int dimension, vector<double>& lower, vector<double>& upper) {
    lower.resize(dimension);
    upper.resize(dimension);

    for (int i = 0; i < dimension; i++) {
        lower[i] = -2.2;
        upper[i] = 1.8;
    }
}

vector<double> getRastriginOptimum(int dimension) {
    return vector<double>(dimension, 0.0);
}