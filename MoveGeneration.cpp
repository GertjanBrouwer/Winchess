#include "MoveGeneration.h"
#include <bitset>
MoveGeneration::MoveGeneration(Board* board)
{
	this->board = board;
}

std::vector<move> MoveGeneration::getAllMoves()
{
	std::vector<move> legalMoves;
	for(uint8_t current = 0; current < 64; current++)
	{
		Piece piece = board->getPieceAt(current);
		if(piece.color != board->turn)
			continue;
		bitboard moves = 0;
		if(piece.type == PieceType::Pawn)
			moves = getPawnMoves(current);
		if(piece.type == PieceType::Knight)
			moves = getKnightMoves(current);
		if(piece.type == PieceType::Rook)
			moves = getRookMoves(current);
		if(moves == 0)
			continue;

		for(uint8_t destination = 0; destination < 64; destination++)
		{
			if(moves & (bitboard)1 << destination & ~board->getOccupied(board->turn))
			{
				legalMoves.push_back({current, destination});
			}
		}
	}

 	return legalMoves;
}

bitboard MoveGeneration::getPawnMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;

	// Standard move
	if(board->turn == PieceColor::White)
		result = piecePosition << 8;
	else
		result = piecePosition >> 8;

	// Double move
	if(piecePosition & kStartPawn & kPieceColor[board->turn])
	{
		if(board->turn == PieceColor::White)
			result |= piecePosition << 16;
		else
			result |= piecePosition >> 16;
	}

	return result;
}

bitboard MoveGeneration::getKnightMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;
	
	result |= (piecePosition << 17) & kNotAFile;
	result |= (piecePosition << 10) & kNotABFile;
	result |= (piecePosition >> 6) & kNotABFile;
	result |= (piecePosition >> 15) & kNotAFile;
	result |= (piecePosition << 15) & kNotHFile;
	result |= (piecePosition << 6) & kNotGHFile;
	result |= (piecePosition >> 10) & kNotGHFile;
	result |= (piecePosition >> 17) & kNotHFile;
	return result;
}

bitboard MoveGeneration::getRookMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;
	result |= northOccluded(piecePosition);
	result |= eastOccluded(piecePosition);
	result |= southOccluded(piecePosition);
	result |= westOccluded(piecePosition);
	return result;
}

bitboard MoveGeneration::northOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	return board;
}

bitboard MoveGeneration::eastOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	return board;
}


bitboard MoveGeneration::southOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	return board;
}

bitboard MoveGeneration::westOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	return board;
}
