#include "Evaluation.h"
#include <intrin.h>
#include <sstream>

int Evaluation::GetPieceBasedEvaluation(Board* board)
{
	int total = 0;
	unsigned int whiteTotal = 0;
	unsigned int blackTotal = 0;

	for (int type = 0; type < 5; type++)
	{
		//count total bits in board white
		whiteTotal += __popcnt64(board->pieces[White][type]) * typeValue.find(type)->second;
		//count total bits in board black
		blackTotal += __popcnt64(board->pieces[Black][type]) * typeValue.find(type)->second;
	}

	//evaluate count and determine
	total += whiteTotal;
	total -= blackTotal;
	
	//return count
	return total;
}

std::string Evaluation::getBoardValue(Board* board)
{
	int value = GetPieceBasedEvaluation(board);
	std::string color = (value > 0) ? "White " : (value == 0) ? "Even board " : "Black ";
	return color + std::to_string(value);
}
