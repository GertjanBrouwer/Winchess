#pragma once
#include <iostream>
#include "Board.h"
#include "MoveGeneration.h"

struct CalculatedMove
{
	double value;
	Move move;
};


class Search
{
	// Function to call alpha-beta to find the best move for the computer
	Move findBestMove(Board* bBoard, int depth, PieceColor computerColor);

	// Function that uses the recursive alpha-beta algorithm to return the value of an updated bitboard
	CalculatedMove alphabeta(Board* board, MoveGeneration* moveGenerator, int depth, int alpha, int beta,
	                         PieceColor computerColor);
};
