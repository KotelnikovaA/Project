#include "DiffEvolution.h"

#include <algorithm>
#include <limits>
#include <cmath>

using namespace std;

DifferentialEvolution::DifferentialEvolution(
    int populationSize,
    double weightFactor,
    double crossoverRate,
    int maxGenerations,
    const vector<double>& lower,
    const vector<double>& upper,
    const function<double(const vector<double>&)>& func,
    double knownOptimum,
    double tol)
    :
    NP(populationSize),
    F(weightFactor),
    CR(crossoverRate),
    M(maxGenerations),
    lowerBounds(lower),
    upperBounds(upper),
    objectiveFunction(func),
    targetValue(knownOptimum),
    tolerance(tol),
    evaluations(0),
    rng(random_device{}())
{
    dimension = lower.size();
    population.resize(NP, Individual(dimension));
}

double DifferentialEvolution::generateRandomDouble(double min, double max)
{
    uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

int DifferentialEvolution::generateRandomInt(int min, int max)
{
    uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void DifferentialEvolution::clampPoint(vector<double>& point)
{
    for (int i = 0; i < dimension; i++)
    {
        if (point[i] < lowerBounds[i])
            point[i] = lowerBounds[i];

        if (point[i] > upperBounds[i])
            point[i] = upperBounds[i];
    }
}

void DifferentialEvolution::initializePopulation()
{
    for (int i = 0; i < NP; i++)
    {
        for (int j = 0; j < dimension; j++)
        {
            population[i].genes[j] =
                generateRandomDouble(lowerBounds[j], upperBounds[j]);
        }

        population[i].functionVal =
            objectiveFunction(population[i].genes);

        evaluations++;
    }
}

vector<double> DifferentialEvolution::mutate(
    int targetIndex,
    const vector<int>& indices)
{
    vector<double> mutant(dimension);

    for (int i = 0; i < dimension; i++)
    {
        mutant[i] =
            population[indices[2]].genes[i] +
            F * (population[indices[0]].genes[i] -
                population[indices[1]].genes[i]);
    }

    clampPoint(mutant);

    return mutant;
}

vector<double> DifferentialEvolution::crossover(
    const vector<double>& target,
    const vector<double>& mutant)
{
    vector<double> trial(dimension);

    int jRand = generateRandomInt(0, dimension - 1);

    for (int i = 0; i < dimension; i++)
    {
        if (generateRandomDouble(0.0, 1.0) <= CR || i == jRand)
            trial[i] = mutant[i];
        else
            trial[i] = target[i];
    }

    return trial;
}

Individual DifferentialEvolution::getBestIndividual()
{
    Individual best = population[0];

    for (int i = 1; i < NP; i++)
    {
        if (population[i].functionVal < best.functionVal)
            best = population[i];
    }

    return best;
}

Individual DifferentialEvolution::optimize()
{
    evaluations = 0;
    history.clear();

    initializePopulation();

    Individual best = getBestIndividual();

    for (int generation = 0; generation < M; generation++)
    {
        for (int targetIdx = 0; targetIdx < NP; targetIdx++)
        {
            vector<int> indices;
            indices.reserve(3);

            while (indices.size() < 3)
            {
                int idx = generateRandomInt(0, NP - 1);

                if (idx != targetIdx &&
                    find(indices.begin(), indices.end(), idx) == indices.end())
                {
                    indices.push_back(idx);
                }
            }

            vector<double> mutant =
                mutate(targetIdx, indices);

            vector<double> trial =
                crossover(population[targetIdx].genes, mutant);

            double trialValue =
                objectiveFunction(trial);

            evaluations++;

            if (trialValue <= population[targetIdx].functionVal)
            {
                population[targetIdx].genes = trial;
                population[targetIdx].functionVal = trialValue;
            }
        }

        best = getBestIndividual();

        double sum = 0.0;
        double worst = -numeric_limits<double>::max();

        for (const auto& ind : population)
        {
            sum += ind.functionVal;

            if (ind.functionVal > worst)
                worst = ind.functionVal;
        }

        GenerationInfo info;
        info.generation = generation;
        info.bestFunctionVal = best.functionVal;
        info.avgFunctionVal = sum / NP;
        info.worstFunctionVal = worst;
        info.bestPoint = best.genes;

        history.push_back(info);

        if (fabs(best.functionVal - targetValue) <= tolerance)
            break;
    }
    best.evaluationsCount = evaluations;
    return best;
}

int DifferentialEvolution::getEvaluations() const
{
    return evaluations;
}