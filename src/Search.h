#pragma once
#include "Board.h"
#include "MoveGeneration.h"
#include <atomic>

struct CalculatedMove
{
	int value;
	Move move;
};

class Search
{
public:
	static std::atomic<bool> ai_thread_running;

	// Function to call alpha-beta to find the best move for the computer
	static Move findBestMove(Board* bBoard, int depthLimit);

	private:
	// Function that uses the recursive alpha-beta algorithm to return the value of an updated bitboard
		static CalculatedMove
		negaMax(Board* board, MoveGeneration* moveGenerator, int depth, int alpha, int beta);
};
