#include "Evaluation.h"
#include <intrin.h>
#include <sstream>

int Evaluation::GetPieceBasedEvaluation(Board* board)
{
	int total = 0;
	unsigned int whiteTotal = 0;
	unsigned int blackTotal = 0;

	whiteTotal = GetPieceBasedEvaluationOfColor(board, White);
	blackTotal = GetPieceBasedEvaluationOfColor(board, Black);

	//evaluate count and determine
	total += whiteTotal;
	total -= blackTotal;
	
	//return count
	return total;
}

int Evaluation::GetPieceBasedEvaluationOfColor(Board* board, PieceColor color)
{
	int total = 0;

	total += __popcnt64(board->pieces[color][Pawn]) * typeValue.find(Pawn)->second;
	total += __popcnt64(board->pieces[color][Knight]) * typeValue.find(Knight)->second;
	total += __popcnt64(board->pieces[color][Bishop]) * typeValue.find(Bishop)->second;
	total += __popcnt64(board->pieces[color][Rook]) * typeValue.find(Rook)->second;
	total += __popcnt64(board->pieces[color][Queen]) * typeValue.find(Queen)->second;

	return total;
}

std::string Evaluation::getBoardValue(Board* board)
{
	std::stringstream ss;
	int value = GetPieceBasedEvaluation(board);
	std::string color = (value > 0) ? "White " : (value == 0) ? "Even board " : "Black ";
	ss << value;
	return color + ss.str();
}
