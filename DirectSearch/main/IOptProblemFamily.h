#pragma once

#include "IGeneralOptProblemFamily.h"
#include "IOptProblem.h"

class IOptProblemFamily : public IGeneralOptProblemFamily
{
protected:
	// The constructor is hidden so that one can not create an object of this class
	// It is required to declare a child, move the constructor to public and implement it
	IOptProblemFamily() {}
public:
	IOptProblem* operator[](int index);
};