#include "MoveGeneration.h"

// Function to calculate the absolute difference
inline int8_t absDiff(int a, int b)
{
	return a < b ? b - a : a - b;
}

inline int getBitIndex(bitboard board)
{
	for (uint8_t pos = 0; pos < 64; ++pos)
	{
		if (board & (bitboard)1 << pos)
		{
			return pos;
		}
	}

	throw _exception();
}

MoveGeneration::MoveGeneration(Board* board)
{
	this->board = board;
}

std::vector<Move> MoveGeneration::getAllMoves()
{
	std::vector<Move> legalMoves;
	// Loop though all squares on the board
	for (uint8_t squareIndex = 0; squareIndex < 64; ++squareIndex)
	{
		Piece piece = board->getPieceAt(squareIndex);
		if (piece.color != board->turn)
			continue;
		bitboard moves = 0;
		if (piece.type == Pawn)
			moves = getPawnMoves(squareIndex);
		if (piece.type == Knight)
			moves = getKnightMoves(squareIndex);
		if (piece.type == Bishop)
			moves = getBishopMoves(squareIndex);
		if (piece.type == Rook)
			moves = getRookMoves(squareIndex);
		if (piece.type == Queen)
			moves = getQueenMoves(squareIndex);
		if (piece.type == King)
			moves = getKingMoves(squareIndex) | getCastlingMoves(squareIndex);
		if (moves == 0)
			continue;

		// Loop though all squares on the board to find indexes of moves
		for (uint8_t destination = 0; destination < 64; ++destination)
		{
			// Check if move result is not colliding with your own pieces
			if (moves & (bitboard)1 << destination & ~board->getOccupied(board->turn))
			{
				// Do move
				board->pieces[board->turn][piece.type] &= ~((bitboard)1 << squareIndex);
				board->pieces[board->turn][piece.type] |= (bitboard)1 << destination;

				bitboard b = board->pieces[board->turn][King];
				int kingPosition = getBitIndex(b);
				if (!isInCheck(kingPosition))
					legalMoves.push_back({squareIndex, destination});

				//Undo move
				board->pieces[board->turn][piece.type] &= ~((bitboard)1 << destination);
				board->pieces[board->turn][piece.type] |= (bitboard)1 << squareIndex;
			}
		}
	}

	return legalMoves;
}

bitboard MoveGeneration::getPawnMoves(uint8_t position)
{
	bitboard enemyOccupied = this->board->getOccupied(1 - this->board->turn);
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	bitboard unoccupied = ~this->board->getAllPieces();

	// Standard move
	if (board->turn == White)
		foundMoves |= unoccupied & piecePosition << 8;
	else
		foundMoves |= unoccupied & piecePosition >> 8;

	// Double move
	if (piecePosition & kStartPawn & kPieceColor[board->turn])
	{
		if (board->turn == White)
			foundMoves |= unoccupied & foundMoves << 8;
		else
			foundMoves |= unoccupied & foundMoves >> 8;
	}

	// Captures move
	if (board->turn == White)
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

bitboard MoveGeneration::getKnightMoves(uint8_t position)
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

bitboard MoveGeneration::getBishopMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	foundMoves |= northEastOccluded(piecePosition);
	foundMoves |= southEastOccluded(piecePosition);
	foundMoves |= northWestOccluded(piecePosition);
	foundMoves |= southWestOccluded(piecePosition);
	return foundMoves;
}

bitboard MoveGeneration::getRookMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard foundMoves = 0;
	foundMoves |= northOccluded(piecePosition);
	foundMoves |= eastOccluded(piecePosition);
	foundMoves |= southOccluded(piecePosition);
	foundMoves |= westOccluded(piecePosition);
	return foundMoves;
}

bitboard MoveGeneration::getQueenMoves(uint8_t position)
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

bitboard MoveGeneration::getKingMoves(uint8_t position)
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

bitboard MoveGeneration::northOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) << 8;
	board |= meUnoccupied & (board & enemyUnoccupied) << 8;
	board |= meUnoccupied & (board & enemyUnoccupied) << 8;
	board |= meUnoccupied & (board & enemyUnoccupied) << 8;
	board |= meUnoccupied & (board & enemyUnoccupied) << 8;
	board |= meUnoccupied & (board & enemyUnoccupied) << 8;
	board |= meUnoccupied & (board & enemyUnoccupied) << 8;
	return board;
}

bitboard MoveGeneration::eastOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) << 1;
	board |= meUnoccupied & (board & enemyUnoccupied) << 1;
	board |= meUnoccupied & (board & enemyUnoccupied) << 1;
	board |= meUnoccupied & (board & enemyUnoccupied) << 1;
	board |= meUnoccupied & (board & enemyUnoccupied) << 1;
	board |= meUnoccupied & (board & enemyUnoccupied) << 1;
	board |= meUnoccupied & (board & enemyUnoccupied) << 1;
	return board;
}

bitboard MoveGeneration::southOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) >> 8;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 8;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 8;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 8;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 8;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 8;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 8;
	return board;
}

bitboard MoveGeneration::westOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) >> 1;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 1;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 1;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 1;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 1;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 1;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 1;
	return board;
}

bitboard MoveGeneration::northEastOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) << 9;
	board |= meUnoccupied & (board & enemyUnoccupied) << 9;
	board |= meUnoccupied & (board & enemyUnoccupied) << 9;
	board |= meUnoccupied & (board & enemyUnoccupied) << 9;
	board |= meUnoccupied & (board & enemyUnoccupied) << 9;
	board |= meUnoccupied & (board & enemyUnoccupied) << 9;
	board |= meUnoccupied & (board & enemyUnoccupied) << 9;
	return board;
}

bitboard MoveGeneration::southEastOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) >> 7;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 7;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 7;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 7;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 7;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 7;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 7;
	return board;
}

bitboard MoveGeneration::northWestOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) << 7;
	board |= meUnoccupied & (board & enemyUnoccupied) << 7;
	board |= meUnoccupied & (board & enemyUnoccupied) << 7;
	board |= meUnoccupied & (board & enemyUnoccupied) << 7;
	board |= meUnoccupied & (board & enemyUnoccupied) << 7;
	board |= meUnoccupied & (board & enemyUnoccupied) << 7;
	board |= meUnoccupied & (board & enemyUnoccupied) << 7;
	return board;
}

bitboard MoveGeneration::southWestOccluded(bitboard board)
{
	bitboard meUnoccupied = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= meUnoccupied & (board & enemyUnoccupied) >> 9;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 9;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 9;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 9;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 9;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 9;
	board |= meUnoccupied & (board & enemyUnoccupied) >> 9;
	return board;
}

// Function to determine if a square is in check
bool MoveGeneration::isInCheck(int square)
{
	//Check if is king
	if (board->pieces[board->turn][King] & (bitboard)1 << square)
	{
		int8_t opponentColor = 1 - board->turn;

		// Pretend the king is a piece and see if it can capture an identical piece of the opposite color
		if (getPawnMoves(square) & board->pieces[opponentColor][Pawn])
			return true;
		if (getKnightMoves(square) & board->pieces[opponentColor][Knight])
			return true;
		if (getRookMoves(square) & board->pieces[opponentColor][Rook])
			return true;
		if (getBishopMoves(square) & board->pieces[opponentColor][Bishop])
			return true;
		if (getQueenMoves(square) & board->pieces[opponentColor][Queen])
			return true;
		if (getKingMoves(square) & board->pieces[opponentColor][King])
			return true;
	}
	return false;
}

bitboard MoveGeneration::getEnPassant(int square)
{
	Move prevMove = board->moves.top();
	bitboard pawns = board->pieces[White][Pawn] | board->pieces[Black][Pawn];

	if (absDiff(prevMove.startPosition / 8, prevMove.targetPosition / 8) == 2 &&
		// Check if opposite moveByChar was length of 2
		pawns & (bitboard)prevMove.targetPosition && // Check if a opposite pawn was moved
		square / 8 == prevMove.targetPosition / 8 && // Check if pawn is on the same row
		absDiff(square % 8, prevMove.targetPosition % 8) == 1) // Check if pawn is next to opposite pawn
	{
		if (prevMove.targetPosition / 8 == 3) // Check if is white
			return (bitboard)1 << prevMove.targetPosition - 8;
		else
			return (bitboard)1 << prevMove.targetPosition + 8;
	}
	return 0;
}

bitboard MoveGeneration::getCastlingMoves(uint8_t square)
{
	bitboard moves = 0;

	// White king castling
	if (square == 4)
	{
		// Make sure that there are no blocking pieces to the queenside
		if (board->castleWQueenSide && !(board->getAllPieces() & 14)) //0000 1110‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[White][King] &= 12; //0000 1100
			board->updateCombinedBitboard();

			if (!isInCheck(2) && !isInCheck(3) && !isInCheck(4))
				moves |= 4; // 0000 0100

			// Remove added kings for check calculation
			board->pieces[White][King] &= ~12; //0000 1100
			board->updateCombinedBitboard();
		}

		// Make sure that there are no blocking pieces to the kingside
		if (board->castleWKingSide && !(board->getAllPieces() & 96)) //0110 0000‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[White][King] &= 96; // 0110‬ 0000
			board->updateCombinedBitboard();

			if (!isInCheck(4) && !isInCheck(5) && !isInCheck(6))
				moves |= 64; // 0100 0000
		}
	}
		// Black king castling
	else if (square == 60)
	{
		// Make sure that there are no blocking pieces to the queenside
		if (board->castleWQueenSide && !(board->getAllPieces() & 8070450532247928832)) //0111 0000‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[Black][King] &= 3458764513820540928; //0011 0000
			board->updateCombinedBitboard();

			if (!isInCheck(2) && !isInCheck(3) && !isInCheck(4))
				moves &= 2305843009213693952; // 0010 0000

			// Remove added kings for check calculation
			board->pieces[Black][King] &= ~3458764513820540928; //0011 0000
			board->updateCombinedBitboard();
		}

		// Make sure that there are no blocking pieces to the kingside
		if (board->castleWKingSide && !(board->getAllPieces() & 432345564227567616)) //0110 0000 ‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[Black][King] &= 432345564227567616; // 0110‬ 0000
			board->updateCombinedBitboard();

			if (!isInCheck(4) && !isInCheck(5) && !isInCheck(6))
				moves &= 144115188075855872; // 0010 0000
		}
	}

	return moves;
}

int MoveGeneration::perft(int depth)
{
	//@TODO FINISH
	bitboard nodes = 0;

	if (depth == 0)
		return 1;

	auto move_list = getAllMoves();
	for (int i = 0; i < move_list.size(); i++)
	{
		board->doMove(move_list[i]);
		int n = board->pieces[board->turn][King];
		if (!isInCheck(log2(n & -n) + 1))
			nodes += perft(depth - 1);
		board->undoMove(move_list[i]);
	}
	return nodes;
}
