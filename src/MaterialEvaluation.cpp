#include "MaterialEvaluation.h"

int MaterialEvaluation::Evaluate(Board* board) 
{
	unsigned int whiteTotal = 
		GetPieceBasedEvaluationOfColor(board, White)
		+ GetBoardEvaluationOfColor(board, White);
	unsigned int blackTotal = 
		GetPieceBasedEvaluationOfColor(board, Black)
		+ GetBoardEvaluationOfColor(board, Black);

	int evaluation = whiteTotal - blackTotal;

	//return evaluation based on the current turns color
	return (board->turn == White ? evaluation : -evaluation);
}


// Returns the evaluation based on where pieces are on the board
int MaterialEvaluation::GetBoardEvaluationOfColor(Board* board, PieceColor color)
{
	int total = 0;

	// Bishop pair, TODO: Does this belong here?
	if (Util::bitCount(board->pieces[color][Bishop]) >= 2)
		total += 50;

	for (auto i = 0; i < 6; i++)
	{
		auto pieces = board->pieces[color][i];
		while (pieces)
		{
			short position = Util::bitIndex(pieces);
			
			// Remove piece from allPieces
			pieces ^= (bitboard)1 << position;
			total += psqt[i][position];
		}
	}
	return total;
}

// Returns the pieces times their respective value | also used in time heuristic, there might be a better way to solve that dependency
int MaterialEvaluation::GetPieceBasedEvaluationOfColor(Board* board, PieceColor color)
{
	return Util::bitCount(board->pieces[color][Pawn]) * typeValue.find(Pawn)->second;
	+ Util::bitCount(board->pieces[color][Knight]) * typeValue.find(Knight)->second;
	+ Util::bitCount(board->pieces[color][Bishop]) * typeValue.find(Bishop)->second;
	+ Util::bitCount(board->pieces[color][Rook]) * typeValue.find(Rook)->second;
	+ Util::bitCount(board->pieces[color][Queen]) * typeValue.find(Queen)->second;
}
