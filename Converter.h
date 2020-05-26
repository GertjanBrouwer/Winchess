#pragma once
#include <string>

#include "MoveGeneration.h"

class Converter
{
public:
	static std::string formatPosition(short position)
	{
		int row = position / 8;
		int column = position % 8;
		char result[] = "  ";
		result[0] = column + 97;
		result[1] = row + 49;
		return std::string(result);
	}

	static std::string formatMove(Move move)
	{
		auto p2 = formatPosition(move.startPosition);
		return formatPosition(move.startPosition) + formatPosition(move.targetPosition);
	}
};
