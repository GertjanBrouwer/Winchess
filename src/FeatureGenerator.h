#pragma once
#include <algorithm>
#include <vector>
#include "Board.h"
#include "MoveGeneration.h"
#include "Util.h"

class FeatureGenerator
{
public:
	FeatureGenerator();
	~FeatureGenerator();
	std::vector<double> generate(Board* board);

private:
	Board* board;
	
	double whiteAttackDefendMaps[64] = {0};
	double blackAttackDefendMaps[64] = {0};

	//TODO: every function start with uppercase
	void attackAndDefendMapsLAVDToSquare(bitboard piece, double relativePieceValue, double attackDefendMap[]);
	double normalizeCoordinate(double coordinate);
	bitboard getKingMoves(int position);
	bitboard getQueenMoves(int position);
	void getQueenMovesWithoutCapture(int position, std::vector<double>& featureVector);
	bitboard getRookMoves(int position);
	void getRookMovesWithoutCapture(int position, std::vector<double>& featureVector);
	bitboard getBishopMoves(int position);
	void getBishopMovesWithoutCapture(int position, std::vector<double>& featureVector);
	bitboard getKnightMoves(int position);
	bitboard getPawnDefend(int position);
	bitboard northOccluded(bitboard piecePosition);
	bitboard eastOccluded(bitboard piecePosition);
	bitboard southOccluded(bitboard piecePosition);
	bitboard westOccluded(bitboard piecePosition);
	bitboard northEastOccluded(bitboard piecePosition);
	bitboard southEastOccluded(bitboard piecePosition);
	bitboard northWestOccluded(bitboard piecePosition);
	bitboard southWestOccluded(bitboard piecePosition);
	double northOccludedWithoutCapture(bitboard piecePosition);
	double eastOccludedWithoutCapture(bitboard piecePosition);
	double southOccludedWithoutCapture(bitboard piecePosition);
	double westOccludedWithoutCapture(bitboard piecePosition);
	double northEastOccludedWithoutCapture(bitboard piecePosition);
	double southEastOccludedWithoutCapture(bitboard piecePosition);
	double northWestOccludedWithoutCapture(bitboard piecePosition);
	double southWestOccludedWithoutCapture(bitboard piecePosition);
	void switchAllyToOpponentPiece(PieceColor color, PieceType pieceType, int pieceIndex);
	void attackAndDefendMaps(PieceColor color, double attackDefendMap[]);
	void pieceCentricPawnFeatures(std::vector<double>& featureVector, PieceColor color);
	void pieceCentricKingFeatures(std::vector<double>& featureVector, PieceColor color);
	void pieceCentricQueenFeatures(std::vector<double>& featureVector, PieceColor color);
	void pieceCentricRookFeatures(std::vector<double>& featureVector, PieceColor color);
	void pieceCentricBishopFeatures(std::vector<double>& featureVector, PieceColor color);
	void pieceCentricKnightFeatures(std::vector<double>& featureVector, PieceColor color);
};