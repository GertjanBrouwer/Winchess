#include "Search.h"

Move Search::findBestMove(Board* board, int depth, PieceColor computerColor)
{
	// Get all the moves available for the computer
	int alpha = INT_MIN, beta = INT_MAX;
	MoveGeneration* moveGenerator = new MoveGeneration(board);

	// depth - 1 : Because the leaf nodes are on depth is 0 instead of 1
	CalculatedMove bestMove = alphabeta(board, moveGenerator, depth - 1, alpha, beta, computerColor);
	std::cout << "Computer value is: " << bestMove.value << std::endl;

	delete moveGenerator;
	return bestMove.move;
}

CalculatedMove
Search::alphabeta(Board* board, MoveGeneration* moveGenerator, int depth, int alpha, int beta, PieceColor computerColor)
{
	// Get all the legal moves for whoever is supposed to move
	std::vector<Move> moves = moveGenerator->getAllMoves();
	Move bestMove = {-1, -1};

	// Stop search if there are no more legal moves or if the search has reached the maximum depth
	if (moves.size() == 0 || depth == 0)
	{
		double board_evaluation = 1.0;
		return {board_evaluation, bestMove};
	}

	// Maximize the value if it is the computer's turn to move
	CalculatedMove best_calculated_move = {};

	// Go through all the legal moves, searching for the move that is worst for the computer
	for(unsigned int moveIndex = 0; moveIndex < moves.size(); ++moveIndex)
	{
		Move move = moves[moveIndex];
		// Update board and recursively call the alpha-beta algorithm
		Board* newBoard = board->getBoardWithMove(move);

		CalculatedMove calculated_move = alphabeta(newBoard, moveGenerator, depth - 1, alpha, beta, computerColor);

		if (board->turn == computerColor)
		{
			// Update the best board value and alpha, the best position the computer is guaranteed of
			if(moveIndex == 0 || calculated_move.value > best_calculated_move.value)
				best_calculated_move = calculated_move;
			if(best_calculated_move.value > alpha)
				alpha = best_calculated_move.value;
		}
		else
		{
			// Update the best board value and beta, the best position the user is guaranteed of
			if(moveIndex == 0 || calculated_move.value < best_calculated_move.value)
				best_calculated_move = calculated_move;
			if(best_calculated_move.value < beta)
				beta = best_calculated_move.value;
		}
		// Stop if the move is worse than all the previous moves
		if (beta <= alpha)
			break;
	}

	return best_calculated_move;
}
