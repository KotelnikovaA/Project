#ifndef RASTRIGIN_H
#define RASTRIGIN_H

#include <vector>

using namespace std;

double rastrigin(const vector<double>& x);

void getRastriginBounds(int dimension, vector<double>& lower, vector<double>& upper);

vector<double> getRastriginOptimum(int dimension);

#endif // RASTRIGIN_H