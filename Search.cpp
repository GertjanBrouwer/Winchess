#include "Search.h"

#include <ctime>
#include "Converter.h"
#include "Evaluation.h"

std::atomic<bool> Search::ai_thread_running(false);

Move Search::findBestMove(Board* board, int depth, PieceColor computerColor)
{
	clock_t begin_time = clock();
	// Get all the moves available for the computer
	int alpha = -2000000000, beta = 2000000000;
	MoveGeneration* moveGenerator = new MoveGeneration(board);

	CalculatedMove bestMove = alphabeta(board, moveGenerator, depth - 1, alpha, beta, computerColor);
	//std::cout << "info string Depth: " << depth << " | Best move: " << Converter::formatMove(bestMove.move) << " Leaf node value: " << bestMove.value << std::endl;

	if(!ai_thread_running)
	{
		return {-1, -1};
	}

	auto time = float(clock() - begin_time) / CLOCKS_PER_SEC * 1000;
	
	std::cout << "info score cp " << bestMove.value * 100 << " depth " << depth  << " nodes  " << bestMove.nodes << " time " << time << " pv " << Converter::formatMove(bestMove.move)
						<< std::endl;
	return bestMove.move;
}

CalculatedMove
Search::alphabeta(Board* board, MoveGeneration* moveGenerator, int depth, int alpha, int beta, PieceColor computerColor)
{
	// Get all the legal moves for whoever is supposed to move
	std::vector<Move> moves = moveGenerator->getAllMoves();

	if(!ai_thread_running)
	{
		return {-1, -1};
	}

	Move bestMove = {-1, -1};
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

	// Stop search if the search has reached the maximum depth
	if (depth == 0)
	{
		double board_evaluation = Evaluation::GetPieceBasedEvaluation(board);
		return {board_evaluation, bestMove , 1};
	}

	// Maximize the value if it is the computer's turn to move
	CalculatedMove best_calculated_move = {};

	int nodes = 0;
	// Go through all the legal moves, searching for the move that is worst for the computer
	for (unsigned int i = 0; i < moves.size(); i++)
	{
		Move move = moves[i];
		// Update bitboard and recursively call the alpha-beta algorithm
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
			if (i == 0 || calculated_move.value > best_calculated_move.value)
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
			if (i == 0 || calculated_move.value < best_calculated_move.value)
				best_calculated_move = calculated_move;
			else if(calculated_move.value == best_calculated_move.value && rand() % 500 == 0)
				best_calculated_move = calculated_move;
			if (best_calculated_move.value < beta)
				beta = best_calculated_move.value;
		}
		// Stop if the move is worse than all the previous moves
		if (beta <= alpha)
		{
			break;
		}
	}
	best_calculated_move.nodes = nodes;
	return best_calculated_move;
}
