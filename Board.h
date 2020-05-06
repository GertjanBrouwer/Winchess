#pragma once
#include <iostream>
#include <map>
#include <stack>
#include <vector>


struct Move
{
	short startPosition;
	short targetPosition;
};

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

const std::map<char, uint8_t> types = {{'p', 0}, {'n', 1}, {'b', 2}, {'r', 3}, {'q', 4}, {'k', 5}};

enum PieceType
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

enum PieceColor
{
	White, 
	Black
};

struct Piece
{
	short color;
	short type;
};

class Board
{
public:
	PieceColor turn = White;
	bitboard pieces[2][6];
	bool wQueenSide = true;
	bool wKingSide = true;
	bool bQueenSide = true;
	bool bKingSide = true;
	std::stack<Move> moves;
	
	Board();
	void printBitboard();
	void moveByChar(const char* move);
	void doMove(Move move);
	void undoMove(Move move);
	bitboard getOccupied(uint8_t color);
	uint8_t positionToIndex(const char* position);
	Piece getPieceAt(uint8_t index);
	void setBoard(std::string fen);
	std::string getFen();
	bitboard getAllPieces();
	void updateCombinedBitboard();
	std::string intToString(int &i);

private:
	void clearBoard();

	char activeColor;
	bool whiteQueenCastle;
	bool whiteKingCastle;
	bool blackQueenCastle;
	bool blackKingCastle;
	std::string enPassant;
	int halfmoveClock;
	int FullmoveNumber;
};
