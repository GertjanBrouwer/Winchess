#pragma once
#include "Board.h"
#include "MoveGeneration.h"
#include <atomic>
#include "Util.h"
#include "Evaluation.h"

const unsigned int MAX_DEPTH = 100;

struct MoveList
{
	int size = 0; // Number of moves in the line.
	Move list[MAX_DEPTH]; // The line.
};

struct CalculatedMove
{
	int value;
	Move move;
	int nodes;
};

class Search
{
public:
	static std::atomic<bool> ai_thread_running;

	// Function to call alpha-beta to find the best move for the computer
	static Move findBestMove(Board* bBoard, Evaluation* evaluator);
	static Move findBestMove(Board* bBoard, Evaluation* evaluator, int depthLimit);

private:
	static int QuiescenceSearch(
		Board* board,
	    MoveGeneration* moveGenerator,
		Evaluation* evaluator,
	    int alpha,
	    int beta,
	    int distanceFromRoot,
	    int depth,
	    MoveList* previousPv);

	// Function that uses the recursive alpha-beta algorithm to return the value of an updated bitboard
	static int negaMax(
		Board* board, 
		MoveGeneration* moveGenerator, 
		Evaluation* evaluator, 
		int depth, 
		int alpha, 
		int beta,               
		int distanceFromRoot,
	    MoveList* previousPv);
};
