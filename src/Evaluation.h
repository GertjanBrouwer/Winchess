#pragma once
#include "Board.h"

//Point values of pieces
//0: Pawn = 1
//1: Knight = 3
//2: Bishop = 3
//3: Rook = 5
//4: Queen = 9
//5: King = 100
const std::map<PieceType, int> typeValue = {
	{Pawn, 100}, {Knight, 320}, {Bishop, 330}, {Rook, 500}, {Queen, 900}, {King, 20000}
};

int bitCount(bitboard board);

class Evaluation
{
public:
	static int GetPieceBasedEvaluation(Board* board);
	static int GetPieceBasedEvaluationOfColor(Board* board, PieceColor color);
	static std::string getBoardValue(Board* board);
};
