#include "MoveGeneration.h"
#include <algorithm>
// Function to calculate the absolute difference
inline int8_t absDiff(int a, int b)
{
	return a < b ? b - a : a - b;
}

MoveGeneration::MoveGeneration(Board* board)
{
	this->board = board;
}

std::vector<Move> MoveGeneration::getAllMoves()
{
	std::vector<Move> legalMoves;
	bitboard allPieces = board->getAllPieces();

	while(allPieces)
	{
		unsigned long pieceIndexResult;
		_BitScanForward64(&pieceIndexResult, allPieces);
		short pieceIndex = pieceIndexResult;
		// List for all legal moves, used to assign move values
		std::vector<Move> tempMoveList;

		// Remove piece from allPieces
		bitboard originMask = (bitboard)1 << pieceIndex;
		allPieces &= ~originMask;

		Piece piece = board->getPieceAt(pieceIndex);

		if(piece.color != board->turn)
			continue;
		bitboard moves = 0;
		bitboard enPassant = 0;
		if(piece.type == Pawn)
		{
			moves = getPawnMoves(pieceIndex);
			// Store enPassant moves separately because the captured piece is determined differently than other 'normal' captures
			enPassant = getEnPassant(pieceIndex);
			moves |= enPassant;
		}
		if(piece.type == Knight)
			moves = getKnightMoves(pieceIndex);
		if(piece.type == Bishop)
			moves = getBishopMoves(pieceIndex);
		if(piece.type == Rook)
			moves = getRookMoves(pieceIndex);
		if(piece.type == Queen)
			moves = getQueenMoves(pieceIndex);
		if(piece.type == King)
		{
			moves = getKingMoves(pieceIndex) | getCastlingMoves(pieceIndex);
		}
		if(moves == 0)
			continue;

		while(moves)
		{
			unsigned long destinationIndex;
			_BitScanForward64(&destinationIndex, moves);
			short destination = destinationIndex;
			// Remove piece from allPieces
			moves &= ~((bitboard)1 << destination);

			bitboard destinationMask = (bitboard)1 << destination;
			// Check if move result is not colliding with your own pieces
			if(destinationMask & ~board->getOccupied(board->turn))
			{
				// Do move
				board->pieces[board->turn][piece.type] &= ~((bitboard)1 << pieceIndex);

				board->pieces[board->turn][piece.type] &= ~originMask;
				board->pieces[board->turn][piece.type] |= destinationMask;

				bitboard pawnPieceRemoved = board->pieces[!board->turn][Pawn] & destinationMask;

				// Remove pawn captured by enPassant moves
				if(enPassant & destinationMask)
				{
					pawnPieceRemoved |= (destinationMask << 8 | destinationMask >> 8) & kCenterRanks;
				}

				board->pieces[!board->turn][Pawn] &= ~pawnPieceRemoved;

				bitboard knightPieceRemoved = board->pieces[!board->turn][Knight] & destinationMask;
				board->pieces[!board->turn][Knight] &= ~knightPieceRemoved;

				bitboard bishopPieceRemoved = board->pieces[!board->turn][Bishop] & destinationMask;
				board->pieces[!board->turn][Bishop] &= ~bishopPieceRemoved;

				bitboard rookPieceRemoved = board->pieces[!board->turn][Rook] & destinationMask;
				board->pieces[!board->turn][Rook] &= ~rookPieceRemoved;

				bitboard queenPieceRemoved = board->pieces[!board->turn][Queen] & destinationMask;
				board->pieces[!board->turn][Queen] &= ~queenPieceRemoved;

				bitboard b = board->pieces[board->turn][King];
				int kingPosition = getBitIndex(b);
				board->updateBitboardCache();

				if(!isInCheck(kingPosition))
				{

					// Promotions
					if(piece.type == Pawn && kOuterRank & destinationMask)
					{
						legalMoves.push_back({pieceIndex, destination, Queen});
						legalMoves.push_back({pieceIndex, destination, Rook});
						legalMoves.push_back({pieceIndex, destination, Bishop});
						legalMoves.push_back({pieceIndex, destination, Knight});
					}
					else
					{
						short score = 0;
						if(pawnPieceRemoved != 0)
							score = 100;
						else if(knightPieceRemoved != 0 || bishopPieceRemoved != 0)
							score = 200;
						else if(rookPieceRemoved != 0)
							score = 300;
						else if(queenPieceRemoved != 0)
							score = 400;

						score += scoreType.find(piece.type)->second;
						tempMoveList.push_back({pieceIndex, destination, 0, score});

						//legalMoves.push_back({pieceIndex, destination});
					}
				}

				//Undo move
				board->pieces[board->turn][piece.type] &= ~destinationMask;
				board->pieces[board->turn][piece.type] |= (bitboard)1 << pieceIndex;

				board->pieces[!board->turn][Pawn] |= pawnPieceRemoved;
				board->pieces[!board->turn][Knight] |= knightPieceRemoved;
				board->pieces[!board->turn][Bishop] |= bishopPieceRemoved;
				board->pieces[!board->turn][Rook] |= rookPieceRemoved;
				board->pieces[!board->turn][Queen] |= queenPieceRemoved;
				board->updateBitboardCache();
			}
		}
		
		//sort the moves based on score
		std::sort(tempMoveList.begin(), tempMoveList.end(), [](const Move& lhs, const Move& rhs) {
			return lhs.score < rhs.score;
		});
		
		for(unsigned int index = 0; index < tempMoveList.size(); ++index)
		{
			legalMoves.push_back(tempMoveList[index]);
		}
	}

	return legalMoves;
}


int MoveGeneration::getBitIndex(bitboard board)
{
	unsigned long pieceIndex;
	_BitScanForward64(&pieceIndex, board);
	return pieceIndex;
}

bitboard MoveGeneration::getPawnMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	bitboard unoccupied = ~this->board->getAllPieces();

	// Standard move
	if(board->turn == White)
		foundMoves |= unoccupied & piecePosition << 8;
	else
		foundMoves |= unoccupied & piecePosition >> 8;

	// Double move
	if(piecePosition & kStartPawn & kPieceColor[board->turn])
	{
		if(board->turn == White)
			foundMoves |= unoccupied & foundMoves << 8;
		else
			foundMoves |= unoccupied & foundMoves >> 8;
	}

	foundMoves |= getPawnCaptures(position);

	return foundMoves;
}

bitboard MoveGeneration::getPawnCaptures(int position)
{
	bitboard enemyOccupied = this->board->getOccupied(!this->board->turn);
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;

	// Captures move
	if(board->turn == White)
	{
		foundMoves |= piecePosition << 7 & enemyOccupied & kNotHFile;
		foundMoves |= piecePosition << 9 & enemyOccupied & kNotAFile;
	}
	else
	{
		foundMoves |= piecePosition >> 7 & enemyOccupied & kNotAFile;
		foundMoves |= piecePosition >> 9 & enemyOccupied & kNotHFile;
	}

	return foundMoves;
}

bitboard MoveGeneration::getKnightMoves(int position)
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

bitboard MoveGeneration::getBishopMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	foundMoves |= northEastOccluded(piecePosition);
	foundMoves |= southEastOccluded(piecePosition);
	foundMoves |= northWestOccluded(piecePosition);
	foundMoves |= southWestOccluded(piecePosition);
	return foundMoves;
}

bitboard MoveGeneration::getRookMoves(int position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	foundMoves |= northOccluded(piecePosition);
	foundMoves |= eastOccluded(piecePosition);
	foundMoves |= southOccluded(piecePosition);
	foundMoves |= westOccluded(piecePosition);
	return foundMoves;
}

bitboard MoveGeneration::getQueenMoves(int position)
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

bitboard MoveGeneration::getKingMoves(int position)
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

bitboard MoveGeneration::northOccluded(bitboard piecePosition)
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

bitboard MoveGeneration::eastOccluded(bitboard piecePosition)
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

bitboard MoveGeneration::southOccluded(bitboard piecePosition)
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

bitboard MoveGeneration::westOccluded(bitboard piecePosition)
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

bitboard MoveGeneration::northEastOccluded(bitboard piecePosition)
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

bitboard MoveGeneration::southEastOccluded(bitboard piecePosition)
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

bitboard MoveGeneration::northWestOccluded(bitboard piecePosition)
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

bitboard MoveGeneration::southWestOccluded(bitboard piecePosition)
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

// Function to determine if a square is in check
bool MoveGeneration::isInCheck(int position)
{
	int8_t opponentColor = !board->turn;

	// Pretend the king is a piece and see if it can capture an identical piece of the opposite color
	if(getPawnCaptures(position) & board->pieces[opponentColor][Pawn])
		return true;
	if(getKnightMoves(position) & board->pieces[opponentColor][Knight])
		return true;
	if(getRookMoves(position) & board->pieces[opponentColor][Rook])
		return true;
	if(getBishopMoves(position) & board->pieces[opponentColor][Bishop])
		return true;
	if(getQueenMoves(position) & board->pieces[opponentColor][Queen])
		return true;
	if(getKingMoves(position) & board->pieces[opponentColor][King])
		return true;
	return false;
}

bitboard MoveGeneration::getEnPassant(int position)
{
	bitboard possibleEnPassantCapture = board->enPassant;
	bitboard piecePosition = ((bitboard)1 << position) & kCenterRanks;
	if(!piecePosition)
		return 0;

	// Captures move
	bitboard pawnCaptures = 0;
	pawnCaptures |= piecePosition << 7 & kNotHFile;
	pawnCaptures |= piecePosition << 9 & kNotAFile;
	pawnCaptures |= piecePosition >> 7 & kNotAFile;
	pawnCaptures |= piecePosition >> 9 & kNotHFile;

	bitboard enPassantCapture = possibleEnPassantCapture & pawnCaptures;
	bitboard resultMove = enPassantCapture;

	return resultMove;
}

bitboard MoveGeneration::getCastlingMoves(int position)
{
	bitboard moves = 0;

	// White king castling
	if(position == 4)
	{
		// Make sure that there are no blocking pieces to the queenside
		if(board->castleWQueenSide && !(board->getAllPieces() & 14)) //0000 1110‬
		{
			if(!isInCheck(2) && !isInCheck(3) && !isInCheck(4))
				moves |= 4; // 0000 0100
		}

		// Make sure that there are no blocking pieces to the kingside
		if(board->castleWKingSide && !(board->getAllPieces() & 96)) //0110 0000‬
		{
			if(!isInCheck(4) && !isInCheck(5) && !isInCheck(6))
				moves |= 64; // 0100 0000
		}
	}

	// Black king castling
	else if(position == 60)
	{
		// Make sure that there are no blocking pieces to the queenside
		if(board->castleBQueenSide && !(board->getAllPieces() & 1008806316530991104)) //0000 1110‬
		{
			if(!isInCheck(58) && !isInCheck(59) && !isInCheck(60))
				moves |= 288230376151711744; // 0000 0100
		}

		// Make sure that there are no blocking pieces to the kingside
		if(board->castleBKingSide && !(board->getAllPieces() & 6917529027641081856)) //0110 0000 ‬
		{
			if(!isInCheck(60) && !isInCheck(61) && !isInCheck(62))
				moves |= 4611686018427387904; // 0100 0000
		}
	}
	board->updateBitboardCache();
	return moves;
}

int MoveGeneration::perft(int depth)
{
	int nodes = 0;
	if(depth == 0)
		return 1;

	std::vector<Move> move_list = getAllMoves();

	for(int i = 0; i < move_list.size(); i++)
	{
		board = board->getBoardWithMove(move_list[i]);

		auto result = perft(depth - 1);
		nodes += result;

		// Undo last move & delete checked board
		Board* original = board->origin;
		delete board;
		board = original;
	}

	return nodes;
}
