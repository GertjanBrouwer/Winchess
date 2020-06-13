#include "FeatureGenerator.h"

FeatureGenerator::FeatureGenerator() {}

FeatureGenerator::~FeatureGenerator()
{
	delete[] whiteAttackDefendMaps;
	delete[] blackAttackDefendMaps;
}

inline void FeatureGenerator::attackAndDefendMapsLAVDToSquare(bitboard piece, double relativePieceValue, double attackDefendMap[])
{
	while(piece)
	{
		short pieceIndex = Util::bitIndex(piece);
		bitboard removeMask = (bitboard)1 << pieceIndex;
		piece &= ~removeMask;
		// Is maxed still usefull here?
		double normalizedAttackDefendValue = std::max(0.0, (1 - (relativePieceValue / 1600.0)));
		attackDefendMap[pieceIndex] = normalizedAttackDefendValue;
	}
}
inline double FeatureGenerator::normalizeCoordinate(double coordinate)
{
	return coordinate * 0.1428;
}

inline void FeatureGenerator::switchAllyToOpponentPiece(PieceColor color, PieceType pieceType, int pieceIndex)
{
	this->board->pieces[color][pieceType] ^= (bitboard)1 << pieceIndex;
	this->board->pieces[static_cast<PieceColor>(1 - color)][pieceType] ^= (bitboard)1 << pieceIndex;
	this->board->turn = static_cast<PieceColor>(1 - color);
	this->board->updateBitboardCache();
}

#pragma region moves
bitboard FeatureGenerator::getKingMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;

	foundMoves |= piecePosition << 8;
	foundMoves |= piecePosition << 9 & kNotAFile;
	foundMoves |= piecePosition << 1 & kNotAFile;
	foundMoves |= piecePosition >> 7 & kNotAFile;
	foundMoves |= piecePosition >> 8;
	foundMoves |= piecePosition >> 9 & kNotHFile;
	foundMoves |= piecePosition >> 1 & kNotHFile;
	foundMoves |= piecePosition << 7 & kNotHFile;

	return foundMoves;
}

bitboard FeatureGenerator::getQueenMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	foundMoves |= northEastOccluded(piecePosition);
	foundMoves |= southEastOccluded(piecePosition);
	foundMoves |= northWestOccluded(piecePosition);
	foundMoves |= southWestOccluded(piecePosition);
	foundMoves |= northOccluded(piecePosition);
	foundMoves |= eastOccluded(piecePosition);
	foundMoves |= southOccluded(piecePosition);
	foundMoves |= westOccluded(piecePosition);
	return foundMoves;
}

void FeatureGenerator::getQueenMovesWithoutCapture(int position, std::vector<double>& featureVector)
{
	bitboard piecePosition = (bitboard)1 << position;

	featureVector.push_back(Util::bitCount(eastOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(westOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(northOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(southOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(northEastOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(northWestOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(southEastOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(southWestOccludedWithoutCapture(piecePosition)) / 7.0);
}

bitboard FeatureGenerator::getRookMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	foundMoves |= northOccluded(piecePosition);
	foundMoves |= eastOccluded(piecePosition);
	foundMoves |= southOccluded(piecePosition);
	foundMoves |= westOccluded(piecePosition);
	return foundMoves;
}

void FeatureGenerator::getRookMovesWithoutCapture(int position, std::vector<double>& featureVector)
{
	bitboard piecePosition = (bitboard)1 << position;

	featureVector.push_back(Util::bitCount(eastOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(westOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(northOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(southOccludedWithoutCapture(piecePosition)) / 7.0);
}

bitboard FeatureGenerator::getBishopMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	foundMoves |= northEastOccluded(piecePosition);
	foundMoves |= southEastOccluded(piecePosition);
	foundMoves |= northWestOccluded(piecePosition);
	foundMoves |= southWestOccluded(piecePosition);
	return foundMoves;
}

void FeatureGenerator::getBishopMovesWithoutCapture(int position, std::vector<double>& featureVector)
{
	bitboard piecePosition = (bitboard)1 << position;

	featureVector.push_back(Util::bitCount(northEastOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(northWestOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(southEastOccludedWithoutCapture(piecePosition)) / 7.0);
	featureVector.push_back(Util::bitCount(southWestOccludedWithoutCapture(piecePosition)) / 7.0);
}

bitboard FeatureGenerator::getKnightMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;

	foundMoves |= piecePosition << 17 & kNotAFile;
	foundMoves |= piecePosition << 10 & kNotABFile;
	foundMoves |= piecePosition >> 6 & kNotABFile;
	foundMoves |= piecePosition >> 15 & kNotAFile;
	foundMoves |= piecePosition << 15 & kNotHFile;
	foundMoves |= piecePosition << 6 & kNotGHFile;
	foundMoves |= piecePosition >> 10 & kNotGHFile;
	foundMoves |= piecePosition >> 17 & kNotHFile;

	return foundMoves;
}

bitboard FeatureGenerator::getPawnDefend(int position)
{
	bitboard enemyOccupied = this->board->getOccupied(!this->board->turn);
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;

	// Captures move
	if(board->turn == White)
	{
		foundMoves |= piecePosition << 7 & kNotHFile;
		foundMoves |= piecePosition << 9 & kNotAFile;
	}
	else
	{
		foundMoves |= piecePosition >> 7 & kNotAFile;
		foundMoves |= piecePosition >> 9 & kNotHFile;
	}

	return foundMoves;
}

bitboard FeatureGenerator::northOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 8;
	return piecePosition;
}

bitboard FeatureGenerator::eastOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 1;
	return piecePosition;
}

bitboard FeatureGenerator::southOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 8;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 8;
	return piecePosition;
}

bitboard FeatureGenerator::westOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 1;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 1;
	return piecePosition;
}

bitboard FeatureGenerator::northEastOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 9;
	return piecePosition;
}

bitboard FeatureGenerator::southEastOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 7;
	return piecePosition;
}

bitboard FeatureGenerator::northWestOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 7;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) << 7;
	return piecePosition;
}

bitboard FeatureGenerator::southWestOccluded(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 9;
	piecePosition |= meUnoccupied & (piecePosition & enemyUnoccupied) >> 9;
	return piecePosition;
}

double FeatureGenerator::northOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition << 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 8);
	return piecePosition;
}

double FeatureGenerator::eastOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition << 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 1);
	return piecePosition;
}

double FeatureGenerator::southOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition >> 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 8);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 8);
	return piecePosition;
}

double FeatureGenerator::westOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition >> 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 1);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 1);
	return piecePosition;
}

double FeatureGenerator::northEastOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition << 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 9);
	return piecePosition;
}

double FeatureGenerator::southEastOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition >> 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 7);
	return piecePosition;
}

double FeatureGenerator::northWestOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition << 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 7);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition << 7);
	return piecePosition;
}

double FeatureGenerator::southWestOccludedWithoutCapture(bitboard piecePosition)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(!this->board->turn);
	piecePosition = meUnoccupied & enemyUnoccupied & (piecePosition >> 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 9);
	piecePosition |= meUnoccupied & enemyUnoccupied & (piecePosition >> 9);
	return piecePosition;
}
#pragma endregion

// Calculates the relatve attack/defend value for each square. The lower valued the piece the better
// relative piece values are magic numbers and should be stored different, they are abitrarily chosen tho
void FeatureGenerator::attackAndDefendMaps(PieceColor color, double attackDefendMap[])
{
	// get bitboards from king to pawns
	bitboard king = board->pieces[color][King];
	bitboard queen = board->pieces[color][Queen];
	bitboard rooks = board->pieces[color][Rook];
	bitboard bishops = board->pieces[color][Bishop];
	bitboard knights = board->pieces[color][Knight];
	bitboard pawns = board->pieces[color][Pawn];

	// Faster to do |= with all moves and after while calculating LAVD, ensures no overwrites.
	while(king)
	{
		short pieceIndex = Util::bitIndex(king);

		bitboard kingMoves = this->getKingMoves(pieceIndex);
		attackAndDefendMapsLAVDToSquare(kingMoves, 1500.0, attackDefendMap);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		king &= ~removeMask;
	}

	while(queen)
	{
		short pieceIndex = Util::bitIndex(queen);

		bitboard queenMoves = this->getQueenMoves(pieceIndex);
		switchAllyToOpponentPiece(color, Queen, pieceIndex);
		queenMoves |= this->getQueenMoves(pieceIndex);
		switchAllyToOpponentPiece(static_cast<PieceColor>(1 - color), Queen, pieceIndex);
		attackAndDefendMapsLAVDToSquare(queenMoves, 900.0, attackDefendMap);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		queen &= ~removeMask;
	}

	while(rooks)
	{
		short pieceIndex = Util::bitIndex(rooks);

		bitboard rookMoves = this->getRookMoves(pieceIndex);
		switchAllyToOpponentPiece(color, Rook, pieceIndex);
		rookMoves |= this->getRookMoves(pieceIndex);
		switchAllyToOpponentPiece(static_cast<PieceColor>(1 - color), Rook, pieceIndex);
		attackAndDefendMapsLAVDToSquare(rookMoves, 500.0, attackDefendMap);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		rooks &= ~removeMask;
	}

	// Pawn and knights can be combined since they have the same score. Combine the same as comment at king^
	while(bishops)
	{
		short pieceIndex = Util::bitIndex(bishops);

		bitboard bishopMoves = this->getBishopMoves(pieceIndex);
		switchAllyToOpponentPiece(color, Bishop, pieceIndex);
		bishopMoves |= this->getBishopMoves(pieceIndex);
		switchAllyToOpponentPiece(static_cast<PieceColor>(1 - color), Bishop, pieceIndex);
		attackAndDefendMapsLAVDToSquare(bishopMoves, 300.0, attackDefendMap);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		bishops &= ~removeMask;
	}

	while(knights)
	{
		short pieceIndex = Util::bitIndex(knights);

		bitboard knightMoves = this->getKnightMoves(pieceIndex);
		attackAndDefendMapsLAVDToSquare(knightMoves, 300.0, attackDefendMap);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		knights &= ~removeMask;
	}

	while(pawns)
	{
		short pieceIndex = Util::bitIndex(pawns);

		bitboard pawnMoves = this->getPawnDefend(pieceIndex);
		attackAndDefendMapsLAVDToSquare(pawnMoves, 100.0, attackDefendMap);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		pawns &= ~removeMask;
	}
}

void FeatureGenerator::pieceCentricPawnFeatures(std::vector<double>& featureVector, PieceColor color)
{
	bitboard pawns = board->pieces[color][Pawn];
	bitboard unassignedPawn = 0;
	// stores if file is occupied and stores the index of the pawn in the pawns bitboard
	// TODO: delete required of this type of object?
	std::tuple<bool, short> assignedFile[8];

	for(int assignIndex = 0; assignIndex < 8; ++assignIndex)
	{
		std::get<0>(assignedFile[assignIndex]) = false;
	}

	while(pawns)
	{
		short pieceIndex = Util::bitIndex(pawns);
		short file = pieceIndex % 8;

		//delete assigned pawn from pawns bitboard
		pawns ^= (bitboard)1 << pieceIndex;

		// If file is occupied skip and assign to nearest file later
		// doing it later prevents occupation by wrong pawn
		if(std::get<0>(assignedFile[file]) == true)
		{
			unassignedPawn |= (bitboard)1 << pieceIndex;
			continue;
		}

		std::get<0>(assignedFile[file]) = true;
		std::get<1>(assignedFile[file]) = pieceIndex;
	}

	// Second run to assign non-assigned pawns (all pawns that occupy and already occupied file)
	while(unassignedPawn)
	{
		short pieceIndex = Util::bitIndex(unassignedPawn);
		short file = pieceIndex % 8;

		short shortestDistance = 8;
		short nearestFile = 0;
		for(short fileIndex = 0; fileIndex < 8; ++fileIndex)
		{
			short distance = abs(file - fileIndex);

			if(std::get<0>(assignedFile[fileIndex]) == false && distance < shortestDistance)
			{
				shortestDistance = distance;
				nearestFile = fileIndex;
			}
		}

		std::get<0>(assignedFile[nearestFile]) = true;
		std::get<1>(assignedFile[nearestFile]) = pieceIndex;

		//delete assigned pawn from pawns bitboard
		unassignedPawn ^= (bitboard)1 << pieceIndex;
	}

	for(int pawnIndex = 0; pawnIndex < 8; ++pawnIndex)
	{
		double exists = 0.0;
		double file = 0.0;
		double rank = 0.0;
		double lva = 0.0;
		double lvd = 0.0;

		if(std::get<0>(assignedFile[pawnIndex]))
		{
			int pieceIndex = std::get<1>(assignedFile[pawnIndex]);

			exists = 1.0;
			file = normalizeCoordinate(pieceIndex % 8);
			// is this working properly?
			rank = normalizeCoordinate(pieceIndex / 8);

			// TODO: This isn't nice, get pointer to ADmap by color?
			if(color == White)
			{
				lva = blackAttackDefendMaps[pieceIndex];
				lvd = whiteAttackDefendMaps[pieceIndex];
			}
			else
			{
				lva = whiteAttackDefendMaps[pieceIndex];
				lvd = blackAttackDefendMaps[pieceIndex];
			}
		}

		featureVector.push_back(exists);
		featureVector.push_back(file);
		featureVector.push_back(rank);
		featureVector.push_back(lva);
		featureVector.push_back(lvd);
	}
}

void FeatureGenerator::pieceCentricKingFeatures(std::vector<double>& featureVector, PieceColor color)
{
	bitboard king = board->pieces[color][King];
	if(!king)
	{
		for(int i = 0; i < 5; ++i)
		{
			featureVector.push_back(0.0);
		}

		return;
	}

	short pieceIndex = Util::bitIndex(king);

	double file = normalizeCoordinate(pieceIndex % 8);
	double rank = normalizeCoordinate(pieceIndex / 8);
	double lva, lvd = 0.0;
	if(color == White)
	{
		lva = blackAttackDefendMaps[pieceIndex];
		lvd = whiteAttackDefendMaps[pieceIndex];
	}
	else
	{
		lva = whiteAttackDefendMaps[pieceIndex];
		lvd = blackAttackDefendMaps[pieceIndex];
	}

	//Exists flag
	featureVector.push_back(1.0);
	featureVector.push_back(file);
	featureVector.push_back(rank);
	featureVector.push_back(lva);
	featureVector.push_back(lvd);
}

void FeatureGenerator::pieceCentricQueenFeatures(std::vector<double>& featureVector, PieceColor color)
{
	bitboard queen = board->pieces[color][Queen];

	// When there is no queen just push 13 zeros
	if(!queen)
	{
		for(int i = 0; i < 13; ++i)
		{
			featureVector.push_back(0.0);
		}

		return;
	}

	// In case of more than one queen it just gets ignored, it is however added in global features
	short pieceIndex = Util::bitIndex(queen);

	double file = normalizeCoordinate(pieceIndex % 8);
	double rank = normalizeCoordinate(pieceIndex / 8);
	double lva, lvd = 0.0;
	if(color == White)
	{
		lva = blackAttackDefendMaps[pieceIndex];
		lvd = whiteAttackDefendMaps[pieceIndex];
	}
	else
	{
		lva = whiteAttackDefendMaps[pieceIndex];
		lvd = blackAttackDefendMaps[pieceIndex];
	}

	//Exists flag
	featureVector.push_back(1.0);
	featureVector.push_back(file);
	featureVector.push_back(rank);
	featureVector.push_back(lva);
	featureVector.push_back(lvd);

	this->getQueenMovesWithoutCapture(pieceIndex, featureVector);
}

void FeatureGenerator::pieceCentricRookFeatures(std::vector<double>& featureVector, PieceColor color)
{
	bitboard rook = board->pieces[color][Rook];

	// At most 2 rooks can be added now, prevent more rook pushing from promotion
	for(int rookIndex = 0; rookIndex < 2; ++rookIndex)
	{
		// When there is no rook just push 9 zeros
		if(!rook)
		{
			for(int i = 0; i < 9; ++i)
			{
				featureVector.push_back(0.0);
			}

			continue;
		}

		// In case of more than two rooks it just gets ignored, it is however added in global features
		short pieceIndex = Util::bitIndex(rook);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		rook &= ~removeMask;

		double file = normalizeCoordinate(pieceIndex % 8);
		double rank = normalizeCoordinate(pieceIndex / 8);
		double lva, lvd = 0.0;
		if(color == White)
		{
			lva = blackAttackDefendMaps[pieceIndex];
			lvd = whiteAttackDefendMaps[pieceIndex];
		}
		else
		{
			lva = whiteAttackDefendMaps[pieceIndex];
			lvd = blackAttackDefendMaps[pieceIndex];
		}

		//Exists flag
		featureVector.push_back(1.0);
		featureVector.push_back(file);
		featureVector.push_back(rank);
		featureVector.push_back(lva);
		featureVector.push_back(lvd);

		this->getRookMovesWithoutCapture(pieceIndex, featureVector);
	}
}

void FeatureGenerator::pieceCentricBishopFeatures(std::vector<double>& featureVector, PieceColor color)
{
	bitboard bishop = board->pieces[color][Bishop];

	// At most 2 bishops can be added now, prevent more bishop pushing from promotion
	for(int bishopIndex = 0; bishopIndex < 2; ++bishopIndex)
	{
		// When there is no bishop just push 9 zeros
		if(!bishop)
		{
			for(int i = 0; i < 9; ++i)
			{
				featureVector.push_back(0.0);
			}

			continue;
		}

		// In case of more than two bishops it just gets ignored, it is however added in global features
		short pieceIndex = Util::bitIndex(bishop);

		bitboard removeMask = (bitboard)1 << pieceIndex;
		bishop &= ~removeMask;

		double file = normalizeCoordinate(pieceIndex % 8);
		double rank = normalizeCoordinate(pieceIndex / 8);
		double lva, lvd = 0.0;
		if(color == White)
		{
			lva = blackAttackDefendMaps[pieceIndex];
			lvd = whiteAttackDefendMaps[pieceIndex];
		}
		else
		{
			lva = whiteAttackDefendMaps[pieceIndex];
			lvd = blackAttackDefendMaps[pieceIndex];
		}

		//Exists flag
		featureVector.push_back(1.0);
		featureVector.push_back(file);
		featureVector.push_back(rank);
		featureVector.push_back(lva);
		featureVector.push_back(lvd);

		this->getBishopMovesWithoutCapture(pieceIndex, featureVector);
	}
}

void FeatureGenerator::pieceCentricKnightFeatures(std::vector<double>& featureVector, PieceColor color)
{
	bitboard knight = board->pieces[color][Knight];

	// At most 2 knights can be added now, prevent more knight pushing from promotion
	for(int knightIndex = 0; knightIndex < 2; ++knightIndex)
	{
		// When there is no bishop just push 5 zeros
		if(!knight)
		{
			for(int i = 0; i < 5; ++i)
			{
				featureVector.push_back(0.0);
			}

			continue;
		}

		// In case of more than two bishops it just gets ignored, it is however added in global features
		short pieceIndex = Util::bitIndex(knight);
		
		bitboard removeMask = (bitboard)1 << pieceIndex;
		knight &= ~removeMask;

		double file = normalizeCoordinate(pieceIndex % 8);
		double rank = normalizeCoordinate(pieceIndex / 8);
		double lva, lvd = 0.0;
		if(color == White)
		{
			lva = blackAttackDefendMaps[pieceIndex];
			lvd = whiteAttackDefendMaps[pieceIndex];
		}
		else
		{
			lva = whiteAttackDefendMaps[pieceIndex];
			lvd = blackAttackDefendMaps[pieceIndex];
		}

		//Exists flag
		featureVector.push_back(1.0);
		featureVector.push_back(file);
		featureVector.push_back(rank);
		featureVector.push_back(lva);
		featureVector.push_back(lvd);
	}
}

// Make the whole thing static? What about parallel search/access?
// The code that calls this function should delete[] featureVector
std::vector<double> FeatureGenerator::generate(Board* board)
{
	this->board = board;

	// ANN is build using float64, thus double used here; unsure if really necessary
	std::vector<double> featureVector;
	featureVector.reserve(353);

	attackAndDefendMaps(White, whiteAttackDefendMaps);
	attackAndDefendMaps(Black, blackAttackDefendMaps);

#pragma region global features

	if(board->turn == White)
		featureVector.push_back(1.0);
	else
		featureVector.push_back(0.0);

	featureVector.push_back(board->castleWQueenSide ? 1.0 : 0.0);
	featureVector.push_back(board->castleWKingSide ? 1.0 : 0.0);
	featureVector.push_back(board->castleBQueenSide ? 1.0 : 0.0);
	featureVector.push_back(board->castleBKingSide ? 1.0 : 0.0);

	// Loop through all pieces on stored in board->pieces | 6 is unique piece count, hardcoded for speed
	for(int pieceTypeIndex = 0; pieceTypeIndex < 6; pieceTypeIndex++)
	{
		// Getting maxTotalOfPieceType can be faster with hardcoded values in key,value structure in board
		int maxTotalOfPieceType = 2;
		if(pieceTypeIndex == Pawn)
			maxTotalOfPieceType = 8;
		if(pieceTypeIndex == Queen || pieceTypeIndex == King)
			maxTotalOfPieceType = 1;

		// Divide by double to prevent int div
		featureVector.push_back(Util::bitCount(board->pieces[White][pieceTypeIndex]) / (double)maxTotalOfPieceType);
		featureVector.push_back(Util::bitCount(board->pieces[Black][pieceTypeIndex]) / (double)maxTotalOfPieceType);
	}

#pragma endregion

#pragma region piece features

	pieceCentricPawnFeatures(featureVector, White);
	pieceCentricPawnFeatures(featureVector, Black);

	pieceCentricKingFeatures(featureVector, White);
	pieceCentricKingFeatures(featureVector, Black);

	pieceCentricQueenFeatures(featureVector, White);
	pieceCentricQueenFeatures(featureVector, Black);

	pieceCentricRookFeatures(featureVector, White);
	pieceCentricRookFeatures(featureVector, Black);

	pieceCentricBishopFeatures(featureVector, White);
	pieceCentricBishopFeatures(featureVector, Black);

	pieceCentricKnightFeatures(featureVector, White);
	pieceCentricKnightFeatures(featureVector, Black);

#pragma endregion

#pragma region square features

	for(int squareIndex = 0; squareIndex < 64; ++squareIndex)
	{
		featureVector.push_back(blackAttackDefendMaps[squareIndex]);
		featureVector.push_back(whiteAttackDefendMaps[squareIndex]);
	}

#pragma endregion

	return featureVector;
}