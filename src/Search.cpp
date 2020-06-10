#include "Search.h"

#include <algorithm>
#include <ctime>
#include <cstring>
#include <fstream>
#include <chrono>
#include "Converter.h"
#include "Evaluation.h"

int nodes;

const unsigned int R = 2; //Null-move reduction depth

std::atomic<bool> Search::ai_thread_running(false);

void PrintSearchInfo(int value, time_t time, int depth, Move bestMove)
{
	std::cout << "info score cp " << value << " depth " << depth << " nodes  " << nodes << " time "
		<< time << " pv " << Converter::formatMove(bestMove) << std::endl;
}

Move Search::findBestMove(Board* board)
{
	int depth = 2;

	MoveGeneration* moveGenerator = new MoveGeneration(board);
	Move bestMove = moveGenerator->getAllMoves({-1, -1})[0];
	delete moveGenerator;

	while (Search::ai_thread_running)
	{
		std::cout << "info depth " << depth << std::endl;

		Move foundMove = Search::findBestMove(board, depth);
		if (foundMove.startPosition == -1)
			// Ignore found move if smaller than 0
			break;

		bestMove = foundMove;
		depth++;
		std::cout << "info currmove " << Converter::formatMove(bestMove) << " currmovenumber " << depth - 1 << std::
			endl;
	}

	return bestMove;
}

MoveList previousBestPv;

Move Search::findBestMove(Board* board, int depthLimit)
{
	nodes = 0;
	auto startTime = std::chrono::steady_clock::now();
	// Get all the moves available for the computer
	int alpha = -2000000000, beta = 2000000000;
	MoveGeneration* moveGenerator = new MoveGeneration(board);

	MoveList bestPv = MoveList();
	// depth - 1 : Because the leaf nodes are on depth is 0 instead of 1
	int val = negaMax(board, moveGenerator, depthLimit - 1, alpha, beta, 0, &bestPv);

	if (!ai_thread_running)
	{
		return {-1, -1};
	}

	auto currentTime = std::chrono::steady_clock::now();
	int time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

	previousBestPv = bestPv;
	Move bestMove = bestPv.list[0];

	PrintSearchInfo(board->turn == White ? val : -val, time, depthLimit, bestMove);

	delete moveGenerator;

	return bestMove;
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

int Search::QuiescenceSearch(Board* board,
                             MoveGeneration* moveGenerator,
                             int alpha,
                             int beta,
                             int depth,
                             int distanceFromRoot,
                             MoveList* previousPv)

{
	if (distanceFromRoot >= MAX_DEPTH) return beta;
	if (!ai_thread_running) return -1;
	int kingPosition = MoveGeneration::getBitIndex(board->pieces[board->turn][King]);
	bool isInCheck = moveGenerator->isInCheck(kingPosition);
	if (isInCheck)
		return negaMax(board, moveGenerator, 1, alpha, beta, distanceFromRoot, previousPv);

	int evaluation = Evaluation::GetPieceBasedEvaluation(board);

	if (evaluation >= beta)
		return beta;
	if (evaluation > alpha)
		alpha = evaluation;

	std::vector<Move> captures = moveGenerator->getCaptureMoves();
	int captureIndex = 0;

	if (distanceFromRoot == 8)
		captureIndex = 0;

	while (captureIndex < captures.size() && captures[captureIndex].score > 0)
	{
		Board* newBoard = board->getBoardWithMove(captures[captureIndex]);
		captureIndex++;
		moveGenerator->board = newBoard;
		int score = -QuiescenceSearch(newBoard, moveGenerator, -beta, -alpha, depth + 1, distanceFromRoot, previousPv);
		if (!ai_thread_running)
			return -1;
		moveGenerator->board = board;
		delete newBoard;

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}

	return alpha;
}

int Search::negaMax(Board* board,
                    MoveGeneration* moveGenerator,
                    int depth,
                    int alpha,
                    int beta,
                    int distanceFromRoot,
                    MoveList* previousPv)
{
	nodes++;
	MoveList pv;

	// Stop search if the search has reached the maximum depth
	if (depth <= 0)
	{
		previousPv->size = 0;
		int board_evaluation = QuiescenceSearch(board, moveGenerator, alpha, beta, 0, distanceFromRoot, &pv);
		//int board_evaluation = Evaluation::GetPieceBasedEvaluation(board);
		return board_evaluation;
	}

	if (!ai_thread_running)
		return 0;

	int nodes = 0;

	moveGenerator->board = board;

	////Null move pruning
	if (NullMoveAllowed(*board, moveGenerator, depth))
	{
		// Do null move
		board->turn = static_cast<PieceColor>(!board->turn);
		bitboard prevEnPassant = board->enPassant;
		board->enPassant = 0;

		int nullMove = -negaMax(board, moveGenerator, depth - 1 - R, -beta, -beta + 1, distanceFromRoot + 1, &pv);

		// Undo null move
		board->turn = static_cast<PieceColor>(!board->turn);
		board->enPassant = prevEnPassant;

		if (nullMove >= beta)
		{
			return beta;
		}
	}
	Move bestPvMove = distanceFromRoot < previousBestPv.size ? previousBestPv.list[distanceFromRoot] : Move{-1, -1};

	// Get all the legal moves for whoever is supposed to move
	std::vector<Move> moves = moveGenerator->getAllMoves(bestPvMove);

	// Stop search if there are no more legal moves
	if (moves.size() == 0)
	{
		bitboard b = board->pieces[board->turn][King];
		int kingPosition = MoveGeneration::getBitIndex(b);

		int board_evaluation = 0;

		if (moveGenerator->isInCheck(kingPosition))
			board_evaluation = -10000 - depth; // Score for checkmate (subtract depth to prioritize closer checkmates)

		return board_evaluation;
	}

	Move bestMove; //used for adding to transposition table later
	int bestScore = alpha;

	// Maximize the value if it is the computer's turn to move
	for (Move move : moves)
	{
		Board* newBoard = board->getBoardWithMove(move);
		moveGenerator->board = newBoard;

		int score = -negaMax(newBoard, moveGenerator, depth - 1, -beta, -alpha, distanceFromRoot + 1, &pv);

		if (!ai_thread_running)
			return -1;

		moveGenerator->board = board;
		delete newBoard;

		if (score > bestScore)
		{
			bestScore = score;
			bestMove = move;
			if (bestScore > alpha)
			{
				alpha = bestScore;
				previousPv->list[0] = move;

				std::memcpy(previousPv->list + 1, pv.list, pv.size * sizeof(Move));
				previousPv->size = pv.size + 1;
			}
		}

		if (alpha >= beta) //Fail high cutoff
			return beta;
	}

	return alpha;
}
