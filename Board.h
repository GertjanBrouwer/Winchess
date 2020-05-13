#pragma once
#include <map>
#include <vector>

struct Move
{
	unsigned int startPosition;
	unsigned int targetPosition;
	short promotionPieceType = 0;
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


const bitboard kCenterRanks = 0b0000000000000000000000011111111111111111000000000000000000000000;

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
	int color;
	int type;
};

struct BoardBitboards
{
	bitboard occupied;
	bitboard occupiedByColor[2];
};

class Board
{
public:
	PieceColor turn = White;
	bitboard pieces[2][6];
	bool castleWQueenSide = true;
	bool castleWKingSide = true;
	bool castleBQueenSide = true;
	bool castleBKingSide = true;
	Move lastMove;
	Board* origin = nullptr;
	BoardBitboards bitboardCache;
	Board();
	Board(Board* board);
	void printBitboard();
	void moveByChar(const char* move);
	void doMove(Move move);
	Board* getBoardWithMove(Move move);
	bitboard getOccupied(int color);
	unsigned int positionToIndex(const char* position);
	Piece getPieceAt(int index);
	void setBoard(std::string fen);
	std::string getFen();
	bitboard getAllPieces();
	void updateBitboardCache();
	bitboard getOccupied(uint8_t color);
	std::string intToString(int& i);

private:
	void clearBoard();
	int halfmoveClock;
	int FullmoveNumber;
};

