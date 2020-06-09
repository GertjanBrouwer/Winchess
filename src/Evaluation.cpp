#include "Evaluation.h"
#include <iostream>
#if _WIN32
#include <intrin.h>
#endif
#include <sstream>

inline int bitCount(bitboard board)
{
#if _WIN32
	return __popcnt64(board);
#else
	return __builtin_popcountll(board);
#endif
}


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
	return (board->turn == White ? total : -total);
}

int Evaluation::GetPieceBasedEvaluationOfColor(Board* board, PieceColor color)
{
	int total = 0;

	total += bitCount(board->pieces[color][Pawn]) * typeValue.find(Pawn)->second;
	total += bitCount(board->pieces[color][Knight]) * typeValue.find(Knight)->second;
	total += bitCount(board->pieces[color][Bishop]) * typeValue.find(Bishop)->second;
	total += bitCount(board->pieces[color][Rook]) * typeValue.find(Rook)->second;
	total += bitCount(board->pieces[color][Queen]) * typeValue.find(Queen)->second;

	return total;
}

std::string Evaluation::getBoardValue(Board* board)
{
	int value = GetPieceBasedEvaluation(board);
	std::string color = (value > 0) ? "White " : (value == 0) ? "Even board " : "Black ";
	return color + std::to_string(value);
}
