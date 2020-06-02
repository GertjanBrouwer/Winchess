#include "Converter.h"

std::string Converter::formatPosition(short position)
{
	int row = position / 8;
	int column = position % 8;
	char result[] = "  ";
	result[0] = column + 97;
	result[1] = row + 49;
	return std::string(result);
}

std::string Converter::formatMove(Move move)
{
	auto p2 = formatPosition(move.startPosition);
	return formatPosition(move.startPosition) + formatPosition(move.targetPosition);
}
