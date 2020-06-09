#include "Search.h"

#include <ctime>
#include <fstream>
#include <chrono>
#include "Converter.h"
#include "Evaluation.h"
#include "TranspositionTable.h"

std::atomic<bool> Search::ai_thread_running(false);

Move Search::findBestMove(Board* board, int depth, PieceColor computerColor)
{
	auto start = std::chrono::steady_clock::now();
	// Get all the moves available for the computer
	int alpha = -2000000000, beta = 2000000000;
	MoveGeneration* moveGenerator = new MoveGeneration(board);

	// depth - 1 : Because the leaf nodes are on depth is 0 instead of 1
	CalculatedMove bestMove = alphabeta(board, moveGenerator, depth - 1, alpha, beta, computerColor);

	if(!ai_thread_running)
	{
		return {-1, -1};
	}

	auto now = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
	
	std::cout << "info score cp " << bestMove.value * 100 << " depth " << depth  << " nodes  " << bestMove.nodes << " time " << time << " pv " << Converter::formatMove(bestMove.move)
						<< std::endl;
	
	delete moveGenerator;
  
	return bestMove.move;
}

CalculatedMove
Search::alphabeta(Board* board, MoveGeneration* moveGenerator, int depth, int alpha, int beta, PieceColor computerColor)
{
	Move bestMove = {-1, -1};

	uint64_t hash = TranspositionTable::globalInstance->hash(board);
	TTEntry entry = TranspositionTable::globalInstance->probe(hash);

	// Stop search if the search has reached the maximum depth
	if(depth == 0)
	{
		double board_evaluation = Evaluation::GetPieceBasedEvaluation(board);
		TranspositionTable::globalInstance->save(hash, bestMove, board_evaluation, depth);
		return {board_evaluation, bestMove, 1};
	}

	if(entry.depth >= depth)
		return {entry.evaluation, entry.move, 1};

	// Get all the legal moves for whoever is supposed to move
	std::vector<Move> moves = moveGenerator->getAllMoves();


	if(!ai_thread_running)
		return {-1, -1};

	// Stop search if there are no more legal moves
	if (moves.size() == 0)
	{
		bitboard b = board->pieces[board->turn][King];
		int kingPosition = MoveGeneration::getBitIndex(b);

		double board_evaluation = 0;

		if (moveGenerator->isInCheck(kingPosition))
			board_evaluation = board->turn == White ? -100 : 100;

		return {board_evaluation, bestMove, 1};
	}

	// Maximize the value if it is the computer's turn to move
	CalculatedMove best_calculated_move = {};

	int nodes = 0;
	// Go through all the legal moves, searching for the move that is worst for the computer
	for(unsigned int moveIndex = 0; moveIndex < moves.size(); ++moveIndex)
	{
		Move move = moves[moveIndex];
		// Update board and recursively call the alpha-beta algorithm
		Board* newBoard = board->getBoardWithMove(move);

		moveGenerator->board = newBoard;
		CalculatedMove calculated_move = alphabeta(newBoard, moveGenerator, depth - 1, alpha, beta, computerColor);

		calculated_move.move = move;
		nodes += calculated_move.nodes;
		
		moveGenerator->board = board;
		delete newBoard;
		// Maximize the value if it is the computer's turn to move
		if (board->turn == White)
		{
			// Update the best board value and alpha, the best position the computer is guaranteed of
			if (moveIndex == 0 || calculated_move.value > best_calculated_move.value)
				best_calculated_move = calculated_move;
			else if(calculated_move.value == best_calculated_move.value && rand() % 500 == 0)
				best_calculated_move = calculated_move;
			if (best_calculated_move.value > alpha)
				alpha = best_calculated_move.value;
		}
		// Minimize the board's value if it is the opponent's turn to move
		else
		{
			// Update the best board value and beta, the best position the user is guaranteed of
			if (moveIndex == 0 || calculated_move.value < best_calculated_move.value)
				best_calculated_move = calculated_move;
			else if(calculated_move.value == best_calculated_move.value && rand() % 500 == 0)
				best_calculated_move = calculated_move;
			if (best_calculated_move.value < beta)
				beta = best_calculated_move.value;
		}
		// Stop if the move is worse than all the previous moves
		if (beta <= alpha)
			break;
	}

	TranspositionTable::globalInstance->save(hash, best_calculated_move.move, best_calculated_move.value, depth);
	best_calculated_move.nodes = nodes;
	return best_calculated_move;
}
