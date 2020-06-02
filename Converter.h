#pragma once
#include <string>

#include "MoveGeneration.h"

class Converter
{
public:
	static std::string formatPosition(short position);
	static std::string formatMove(Move move);
};
