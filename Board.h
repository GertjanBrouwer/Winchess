#pragma once
#include <iostream>

typedef uint64_t bitboard;

const bitboard kStartPawn = 0b0000000011111111000000000000000000000000000000001111111100000000;
const bitboard kStartKnight = 0b0100001000000000000000000000000000000000000000000000000001000010;
const bitboard kStartBishop = 0b0010010000000000000000000000000000000000000000000000000000100100;
const bitboard kStartRook = 0b1000000100000000000000000000000000000000000000000000000010000001;
const bitboard kStartQueen = 0b0000100000000000000000000000000000000000000000000000000000001000;
const bitboard kStartKing = 0b0001000000000000000000000000000000000000000000000000000000010000;

const bitboard kStartAllWhite = 0b0000000000000000000000000000000000000000000000001111111111111111;
const bitboard kStartAllBlack = 0b1111111111111111000000000000000000000000000000000000000000000000;

const bitboard kStartPiecePositions[] = {kStartPawn, kStartKnight, kStartBishop, kStartRook, kStartQueen, kStartKing};
const bitboard kPieceColor[] = {kStartAllWhite, kStartAllBlack};

const char kPieceChars[] = {'p', 'n', 'b', 'r', 'q', 'k'};

struct piece
{
	short color;
	short type;
};

class Board
{
public:
	bitboard pieces[2][6];

	Board();
	void printBitboard();
	void move(const char* move);
	uint8_t positionToIndex(const char* position);
	piece getPieceAt(uint8_t index);
	void setBoard(const char* fen);
	std::string getFen();
	std::string toString(int &i);

private:

	void clearBoard();
};
