#include "DirectSearch.h"
#include <iostream>
#include <chrono>

using namespace std;

void DirectSearch::directSearch(int currentDimension,
                                vector<double>& currentPoint,
                                const vector<double>& lower,
                                const vector<double>& upper,
                                double step,
                                const function<double(const vector<double>&)>& func,
                                Point& best) const {

    int dimension = (int)lower.size();

    if (currentDimension == dimension) {
        double currentValue = func(currentPoint);
        if (currentValue < best.value) {
            best.point = currentPoint;
            best.value = currentValue;
        }
        return;
    }

    int steps = static_cast<int>((upper[currentDimension] - lower[currentDimension]) / step) + 1;

    for (int i = 0; i < steps; i++) {
        currentPoint[currentDimension] = lower[currentDimension] + i * step;

        if (i == steps - 1) {
            currentPoint[currentDimension] = upper[currentDimension];
        }

        directSearch(currentDimension + 1, currentPoint, lower, upper, step, func, best);
    }
}

Point DirectSearch::findMinimum(const vector<double>& lower,
                                const vector<double>& upper,
                                double step,
                                const function<double(const vector<double>&)>& func) const {

    int dimension = (int)lower.size();

    Point best(dimension);
    best.value = numeric_limits<double>::max();

    vector<double> currentPoint(dimension);

    directSearch(0, currentPoint, lower, upper, step, func, best);

    return best;
}