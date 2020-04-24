#include "MoveGeneration.h"

MoveGeneration::MoveGeneration(Board* board)
{
	this->board = board;
}

std::vector<move> MoveGeneration::GetAllMoves(uint8_t color)
{
	std::vector<move> legalMoves;
	for(uint8_t current = 0; current < 64; current++)
	{
		Piece piece = board->getPieceAt(current);
		if(piece.color != color)
			continue;
		bitboard moves = 0;
		if(piece.type == PieceType::Pawn)
			moves = GetPawnMoves(current, color);
		if(piece.type == PieceType::Knight)
			moves = GetKnightMoves(current);
		if(moves == 0)
			continue;

		for(uint8_t destination = 0; destination < 64; destination++)
		{
			if(moves & (bitboard)1 << destination & board->getOccupiedBoard(color))
			{
				legalMoves.push_back({current, destination});
			}
		}
	}

	return legalMoves;
}

bitboard MoveGeneration::GetPawnMoves(uint8_t position, uint8_t color)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;
	// Standard move

	if(color)
		result = piecePosition >> 8;
	else
		result = piecePosition << 8;

	// Double move
	if(piecePosition & kStartPawn & kPieceColor[color])
	{
		if(color)
			result |= piecePosition >> 16;
		else
			result |= piecePosition << 16;
	}

	return result;
}

bitboard MoveGeneration::GetKnightMoves(uint8_t position)
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
