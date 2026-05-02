#ifndef HILL_FUNCTION_H
#define HILL_FUNCTION_H

#include <vector>
#include "HillProblem.h"

using namespace std;

double hillFunction(int problemIndex, const vector<double>& x);

void getHillBounds(vector<double>& lower, vector<double>& upper);

vector<double> getHillOptimum(int problemIndex, double& optimumValue);

#endif // HILL_FUNCTION_HPP