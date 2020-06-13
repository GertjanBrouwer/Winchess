#pragma once
#include "Board.h"

class Evaluation
{
public:
	virtual int Evaluate(Board* board) = 0;
};
