#pragma once
#include <iostream>
#include <vector>
#include "Board.h"

const bitboard kNotAFile = 0b1111111011111110111111101111111011111110111111101111111011111110;
const bitboard kNotABFile = 0b1111110011111100111111001111110011111100111111001111110011111100;
const bitboard kNotHFile = 0b0111111101111111011111110111111101111111011111110111111101111111;
const bitboard kNotGHFile = 0b0011111100111111001111110011111100111111001111110011111100111111;

class MoveGeneration
{
private:
	Board* board;

	bitboard getPawnMoves(uint8_t position);
	bitboard getKnightMoves(uint8_t position);
	bitboard getBishopMoves(uint8_t position);
	bitboard getRookMoves(uint8_t position);
	bitboard getQueenMoves(uint8_t position);
	bitboard getKingMoves(uint8_t position);

	bitboard northOccluded(bitboard board);
	bitboard eastOccluded(bitboard board);
	bitboard southOccluded(bitboard board);
	bitboard westOccluded(bitboard board);

	bitboard northEastOccluded(bitboard board);
	bitboard southEastOccluded(bitboard board);
	bitboard northWestOccluded(bitboard board);
	bitboard southWestOccluded(bitboard board);

	bitboard getEnPassant(int square);
	bitboard getCastlingMoves(uint8_t square);

public:
	MoveGeneration(Board* board);
	std::vector<Move> getAllMoves();

	bool isInCheck(int square);

	int perft(int depth);
};
