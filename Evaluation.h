#pragma once
#include "Board.h"

//Point values of pieces
//0: Pawn = 1
//1: Knight = 3
//2: Bishop = 3
//3: Rook = 5
//4: Queen = 9
//5: King = 100
const std::map<char, uint8_t> typeValue = {{0, 1}, {1, 3}, {2, 3}, {3, 5}, {4, 9}, {5, 100}};

class Evaluation
{
public:
	static int GetPieceBasedEvaluation(Board* board);
	static int GetPieceBasedEvaluationOfColor(Board* board);
	static std::string getBoardValue(Board* board);
};