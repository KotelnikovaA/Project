#pragma once

#include "IOptProblemFamily.h"
#include "ShekelProblem.h"

// Shekel problem family
class TShekelProblemFamily : public IOptProblemFamily
{
public:
    TShekelProblemFamily() : IOptProblemFamily()
    {
        for (int i = 0; i < NUM_SHEKEL_PROBLEMS; i++)
        {
            pOptProblems.push_back(new TShekelProblem(i));
        }
    }
};