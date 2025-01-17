#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

#include "Board.h"
#include "MoveGeneration.h"

enum Flag
{
	Lowerbound,
	Exact,
	Upperbound
};

struct TTEntry
{
	uint64_t zobrist;
	Move move;
	int evaluation;
	int depth;
	Flag flag;
};

class TranspositionTable
{
public:
	static TranspositionTable* globalInstance;
	TranspositionTable();
	uint64_t hash(Board* board);
	uint64_t getHashOfPiece(PieceType type, PieceColor color);
	void clear();
	void save(uint64_t key, Move move, int evaluation, int depth, Flag flag);
	TTEntry probe(uint64_t key);

private:
	uint64_t pieceKeys[64][12];
	std::vector<uint64_t> remainderKeys;
	Board* board;
	int tableSize = 1;
	TTEntry* transpositionTable;
};