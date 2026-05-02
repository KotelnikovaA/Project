#pragma once

#include "grishagin_function.h"
#include "IOptProblemFamily.h"

class TGrishaginProblemFamily : public IOptProblemFamily
{
public:
    TGrishaginProblemFamily() : IOptProblemFamily()
    {
        for (int i = 1; i <= 100; i++)
            pOptProblems.push_back(new TGrishaginProblem(i));
    }
};