#ifndef DIFFERENTIAL_EVOLUTION_H
#define DIFFERENTIAL_EVOLUTION_H

#include <vector>
#include <functional>
#include <random>

using namespace std;

struct Individual
{
    vector<double> genes;
    double functionVal;
    int evaluationsCount;

    Individual(int dimension = 0)
        : genes(dimension, 0.0), functionVal(0.0) {
    }
};

class DifferentialEvolution
{
private:
    int NP;
    double F;
    double CR;
    int M;

    vector<Individual> population;
    mt19937 rng;

    vector<double> lowerBounds;
    vector<double> upperBounds;
    int dimension;

    function<double(const vector<double>&)> objectiveFunction;

    double targetValue;
    double tolerance;

    int evaluations;

    double generateRandomDouble(double min, double max);
    int generateRandomInt(int min, int max);
    void clampPoint(vector<double>& point);

public:

    struct GenerationInfo
    {
        int generation;
        double bestFunctionVal;
        double avgFunctionVal;
        double worstFunctionVal;
        vector<double> bestPoint;
    };

    vector<GenerationInfo> history;

    DifferentialEvolution(
        int populationSize,
        double weightFactor,
        double crossoverRate,
        int maxGenerations,
        const vector<double>& lower,
        const vector<double>& upper,
        const function<double(const vector<double>&)>& func,
        double knownOptimum,
        double tolerance
    );

    void initializePopulation();
    vector<double> mutate(int targetIndex, const vector<int>& indices);
    vector<double> crossover(
        const vector<double>& target,
        const vector<double>& mutant);

    Individual optimize();
    Individual getBestIndividual();

    int getEvaluations() const;
};

#endif