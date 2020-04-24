#pragma once
#include <iostream>
#include <vector>
#include "Board.h"

const bitboard kNotAFile = 0b1111111011111110111111101111111011111110111111101111111011111110;
const bitboard kNotABFile = 0b1111110011111100111111001111110011111100111111001111110011111100;
const bitboard kNotHFile = 0b0111111101111111011111110111111101111111011111110111111101111111;
const bitboard kNotGHFile = 0b0011111100111111001111110011111100111111001111110011111100111111;

struct move
{
	short startPosition;
	short targetPosition;
};

class MoveGeneration
{
public:
	Board* board;

	MoveGeneration(Board *board);

	std::vector<move> getAllMoves();
	bitboard getPawnMoves(uint8_t position);
	bitboard getKnightMoves(uint8_t position);
	bitboard getRookMoves(uint8_t position);

	bitboard northOccluded(bitboard board);
	bitboard eastOccluded(bitboard board);
	bitboard southOccluded(bitboard board);
	bitboard westOccluded(bitboard board);

	bitboard northEastOccluded
};
