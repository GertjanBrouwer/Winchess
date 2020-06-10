#include "Search.h"
#include "Converter.h"
#include "Evaluation.h"
#include "TranspositionTable.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <cmath>

int nodes;
const unsigned int R = 2; //Null-move reduction depth
std::atomic<bool> Search::ai_thread_running(false);

void PrintSearchInfo(int value, time_t time, int depth, CalculatedMove bestMove)
{
	std::cout << "info score cp " << value << " depth " << depth << " nodes  " << nodes << " time " << time << " pv "
						<< Converter::formatMove(bestMove.move) << std::endl;
}

Move Search::findBestMove(Board* board, int depthLimit)
{
	nodes = 0;
	auto startTime = std::chrono::steady_clock::now();
	// Get all the moves available for the computer
	int alpha = -2000000000, beta = 2000000000;
	MoveGeneration* moveGenerator = new MoveGeneration(board);

	// depth - 1 : Because the leaf nodes are on depth is 0 instead of 1
	CalculatedMove bestMove = negaMax(board, moveGenerator, depthLimit - 1, alpha, beta);

	if(!ai_thread_running)
	{
		return {-1, -1};
	}

	auto currentTime = std::chrono::steady_clock::now();
	int time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

	PrintSearchInfo(board->turn == White ? bestMove.value : -bestMove.value, time, depthLimit, bestMove);

	delete moveGenerator;

	return bestMove.move;
}

inline bool NullMoveAllowed(Board& board, MoveGeneration* move_generation, int depth)
{
	int kingPosition = MoveGeneration::getBitIndex(board.pieces[board.turn][King]);
	bool isInCheck = move_generation->isInCheck(kingPosition);
	return !isInCheck &&
				 //don't drop directly into null move pruning
				 depth > R + 1
				 //avoid null move pruning in very late game positions due to zanauag issues. Even with verification search e.g 8/6k1/8/8/8/8/1K6/Q7 w - - 0 1
				 && bitCount(board.getAllPieces()) >= 5;
}

CalculatedMove Search::negaMax(Board* board, MoveGeneration* moveGenerator, int depth, int alpha, int beta)
{
	int alphaOrig = alpha;
	Move bestMove = {-1, -1};
	nodes++;

	uint64_t hash = TranspositionTable::globalInstance->hash(board);
	TTEntry entry = TranspositionTable::globalInstance->probe(hash);

	if(entry.depth >= depth)
	{
		if(entry.bound == Exact)
			return {entry.evaluation, entry.move};
		else if(entry.bound == Lowerbound)
			alpha = std::max(entry.alpha, entry.evaluation);
		else if(entry.bound == Upperbound)
			beta = std::min(entry.beta, entry.evaluation);

		if(alpha >= beta)
			return {entry.evaluation, entry.move};
	}

	// Stop search if the search has reached the maximum depth
	if(depth <= 0)
	{
		int board_evaluation = Evaluation::GetPieceBasedEvaluation(board);
		return {board_evaluation, bestMove};
	}

	if(!ai_thread_running)
		return {-1, -1};

	int nodes = 0;

	moveGenerator->board = board;

	////Null move pruning
	if(NullMoveAllowed(*board, moveGenerator, depth))
	{
		// Do null move
		board->turn = static_cast<PieceColor>(!board->turn);
		bitboard prevEnPassant = board->enPassant;
		board->enPassant = 0;

		CalculatedMove bestNullMove = negaMax(board, moveGenerator, depth - 1 - R, -beta, -beta + 1);

		// Undo null move
		board->turn = static_cast<PieceColor>(!board->turn);
		board->enPassant = prevEnPassant;

		if(-bestNullMove.value >= beta)
		{
			return {beta, {-1, -1}};
		}
	}

	// Get all the legal moves for whoever is supposed to move
	std::vector<Move> moves = moveGenerator->getAllMoves();

	// Stop search if there are no more legal moves
	if(moves.size() == 0)
	{
		bitboard b = board->pieces[board->turn][King];
		int kingPosition = MoveGeneration::getBitIndex(b);

		int board_evaluation = 0;

		if(moveGenerator->isInCheck(kingPosition))
			board_evaluation = -10000 - depth; // Score for checkmate (subtract depth to prioritize closer checkmates)

		return {board_evaluation, bestMove};
	}

	// Maximize the value if it is the computer's turn to move
	CalculatedMove best_calculated_move = {};
	best_calculated_move.value = -20000;
	for(Move move : moves)
	{
		Board* newBoard = board->getBoardWithMove(move);
		moveGenerator->board = newBoard;

		CalculatedMove calculated_move = negaMax(newBoard, moveGenerator, depth - 1, -beta, -alpha);

		if(!ai_thread_running)
			return {-1, -1};

		moveGenerator->board = board;
		calculated_move.value = -calculated_move.value;
		calculated_move.move = move;

		delete newBoard;

		if(calculated_move.value > best_calculated_move.value)
			best_calculated_move = calculated_move;

		if(best_calculated_move.value > alpha)
			alpha = calculated_move.value;

		if(alpha >= beta)
			return {beta, move};
	}
	Bound bound;
	if(best_calculated_move.value <= alphaOrig)
		bound = Upperbound;
	else if (best_calculated_move.value >= beta)
		bound = Lowerbound; 
	else 
		bound = Exact;
	TranspositionTable::globalInstance->save(
			hash, best_calculated_move.move, best_calculated_move.value, depth, alpha, beta, bound);
	return best_calculated_move;
}
