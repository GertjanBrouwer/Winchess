#pragma once
#include "Board.h"
#include "MoveGeneration.h"

struct CalculatedMove
{
	double value;
	Move move;
	int nodes;
};


class Search
{

public:
	static Move findBestMove(Board* bBoard, int depth, PieceColor computerColor);

private:
	static CalculatedMove alphabeta(Board* board, MoveGeneration* moveGenerator, int depth, int alpha, int beta,
	                         PieceColor computerColor);
};
