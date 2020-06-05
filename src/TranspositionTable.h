#pragma once
#include <iostream>
#include <vector>

#include "Board.h"
#include "MoveGeneration.h"

struct TTEntry
{
	Move move;
	double evaluation;
	int depth;
};

class TranspositionTable
{
public:
	static TranspositionTable* globalInstance;
	TranspositionTable();
	uint64_t hash(Board* board);
	uint64_t getHashOfPiece(PieceType type, PieceColor color);
	void clear();
	void save(uint64_t key, Move move, double evaluation, int depth);
	TTEntry probe(uint64_t key);

private:
	uint64_t pieceKeys[64][12];
	std::vector<uint64_t> otherKeys;
	Board* board;
	std::map<uint64_t, TTEntry> transpositionTable;
};