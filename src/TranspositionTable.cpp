#include "TranspositionTable.h"
#include <random>
#include <cstring>

TranspositionTable* TranspositionTable::globalInstance = nullptr;

TranspositionTable::TranspositionTable()
{
	this->board = board;
	transpositionTable = new TTEntry[tableSize];
	std::default_random_engine generator(128612482);
	std::uniform_int_distribution<uint64_t> distribution;

	for(int square = 0; square < 64; ++square)
	{
		std::vector<uint64_t> keys;
		for(int piece = 0; piece < 12; ++piece)
		{
			pieceKeys[square][piece] = distribution(generator);
		}
	}

	// Current color
	otherKeys.push_back(distribution(generator));
	// Enpassant
	otherKeys.push_back(distribution(generator));
	// White kingside castle
	otherKeys.push_back(distribution(generator));
	// Black kingside castle
	otherKeys.push_back(distribution(generator));
	// White queenside castle
	otherKeys.push_back(distribution(generator));
	// Black queenside castle
	otherKeys.push_back(distribution(generator));
}

uint64_t TranspositionTable::getHashOfPiece(PieceType type, PieceColor color)
{
	uint64_t hash = 0;
	bitboard pieces = board->pieces[color][type];

	while(pieces)
	{
		int squareIndex = MoveGeneration::getBitIndex(pieces);

		bitboard originMask = (bitboard)1 << squareIndex;
		pieces &= ~originMask;

		int pieceIndex = color == White ? type : type + 6;

		hash ^= pieceKeys[squareIndex][pieceIndex];
	}

	return hash;
}

void TranspositionTable::clear()
{
	std::memset(transpositionTable, 0, tableSize * sizeof(TTEntry));
}

void TranspositionTable::save(uint64_t key, Move move, int evaluation, int depth, int alpha, int beta, Bound bound)
{
	TTEntry newEntry = {key, move, evaluation, depth, alpha, beta, bound};
	transpositionTable[key % tableSize] = newEntry;
}

TTEntry TranspositionTable::probe(uint64_t key)
{
	TTEntry entry = transpositionTable[key % tableSize];

	if(entry.zobrist != key)
		return {uint16_t(1), {-1, -1}, -1, -1};

	return entry;
}

uint64_t TranspositionTable::hash(Board* board)
{
	this->board = board;
	uint64_t hash = 0;

	hash ^= getHashOfPiece(Pawn, White);
	hash ^= getHashOfPiece(Pawn, Black);

	hash ^= getHashOfPiece(Bishop, White);
	hash ^= getHashOfPiece(Bishop, Black);

	hash ^= getHashOfPiece(Knight, White);
	hash ^= getHashOfPiece(Knight, Black);

	hash ^= getHashOfPiece(Rook, White);
	hash ^= getHashOfPiece(Rook, Black);

	hash ^= getHashOfPiece(Queen, White);
	hash ^= getHashOfPiece(Queen, Black);

	hash ^= getHashOfPiece(King, White);
	hash ^= getHashOfPiece(King, Black);

	if(board->turn == Black)
		hash ^= otherKeys[0];
	if(board->enPassant)
	{
		int squareIndex = MoveGeneration::getBitIndex(board->enPassant);
		int rank = squareIndex / 8;
		int file = squareIndex % 8;
		hash ^= uint64_t(rank ^ file) ^ otherKeys[1];	
	}
	if(board->castleWKingSide)
		hash ^= otherKeys[2];
	if(board->castleBKingSide)
		hash ^= otherKeys[3];
	if(board->castleWQueenSide)
		hash ^= otherKeys[4];
	if(board->castleBQueenSide)
		hash ^= otherKeys[5];

	return hash;
}
