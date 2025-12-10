#ifndef DIRECT_SEARCH_H
#define DIRECT_SEARCH_H

#include <vector>
#include <functional>
#include <limits> 

using namespace std;

struct Point {
    vector<double> point;
    double value;

    Point(int dimension = 0) {
        if (dimension > 0) {
            point.resize(dimension);
        }
        value = numeric_limits<double>::max();
    }
};

class DirectSearch {
private:
    void directSearch(int currentDimension,
                     vector<double>& currentPoint,
                     const vector<double>& lower,
                     const vector<double>& upper,
                     double step,
                     const function<double(const vector<double>&)>& func,
                     Point& best) const;
public:
    Point findMinimum(const vector<double>& lower,
                      const vector<double>& upper,
                      double step,
                      const function<double(const vector<double>&)>& func) const;
};

#endif // DIRECT_SEARCH_H