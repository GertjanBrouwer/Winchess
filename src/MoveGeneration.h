#pragma once
#include <vector>
#include <algorithm>
#include "Board.h"
#include "Util.h"

const bitboard kNotAFile = 0b1111111011111110111111101111111011111110111111101111111011111110;
const bitboard kNotABFile = 0b1111110011111100111111001111110011111100111111001111110011111100;
const bitboard kNotHFile = 0b0111111101111111011111110111111101111111011111110111111101111111;
const bitboard kNotGHFile = 0b0011111100111111001111110011111100111111001111110011111100111111;

const bitboard kOuterRank = 0b1111111100000000000000000000000000000000000000000000000011111111;

const std::map<char, uint8_t> scoreType = {{Pawn, 1}, {Knight, 2}, {Bishop, 3}, {Rook, 4}, {Queen, 5}, {King, 6}};

class MoveGeneration
{
public:
	bitboard getPawnMoves(int position);
	bitboard getPawnCaptures(int position);
	bitboard getKnightMoves(int position);
	bitboard getBishopMoves(int position);
	bitboard getRookMoves(int position);
	bitboard getQueenMoves(int position);
	bitboard getKingMoves(int position);

	bitboard northOccluded(bitboard board);
	bitboard eastOccluded(bitboard board);
	bitboard southOccluded(bitboard board);
	bitboard westOccluded(bitboard board);

	bitboard northEastOccluded(bitboard board);
	bitboard southEastOccluded(bitboard board);
	bitboard northWestOccluded(bitboard board);
	bitboard southWestOccluded(bitboard board);

	#pragma region FeatureGeneration
	void getBishopMovesWithoutCapture(int position, std::vector<double>& featureVector);
	void getRookMovesWithoutCapture(int position, std::vector<double>& featureVector);
	void getQueenMovesWithoutCapture(int position, std::vector<double>& featureVector);

	double northOccludedWithoutCapture(bitboard board);
	double eastOccludedWithoutCapture(bitboard board);
	double southOccludedWithoutCapture(bitboard board);
	double westOccludedWithoutCapture(bitboard board);

	double northEastOccludedWithoutCapture(bitboard board);
	double southEastOccludedWithoutCapture(bitboard board);
	double northWestOccludedWithoutCapture(bitboard board);
	double southWestOccludedWithoutCapture(bitboard board);
	#pragma endregion

	bitboard getEnPassant(int square);
	bitboard getCastlingMoves(int square);

	Board* board;
	MoveGeneration(Board* board);
	std::vector<Move> getAllMoves(Move pv, bool capturesOnly = false);
	std::vector<Move> getCaptureMoves();
	
	bool isInCheck(int square);
	int perft(int depth);
};
