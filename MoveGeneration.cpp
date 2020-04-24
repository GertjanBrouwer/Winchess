#include "MoveGeneration.h"
#include <bitset>

MoveGeneration::MoveGeneration(Board* board)
{
	this->board = board;
}

// Function to calculate the absolute difference
inline int8_t absDiff(int a, int b)
{
	return (a < b) ? b - a : a - b;
}


std::vector<move> MoveGeneration::getAllMoves()
{
	std::vector<move> legalMoves;
	for (uint8_t current = 0; current < 64; current++)
	{
		Piece piece = board->getPieceAt(current);
		if (piece.color != board->turn)
			continue;
		bitboard moves = 0;
		if(piece.type == Pawn)
			moves = getPawnMoves(current);
		if(piece.type == Knight)
			moves = getKnightMoves(current);
		if(piece.type == Bishop)
			moves = getBishopMoves(current);
		if(piece.type == Rook)
			moves = getRookMoves(current);
		if(piece.type == Queen)
			moves = getQueenMoves(current);
		if(piece.type == King)
			moves = getKingMoves(current);
		if (moves == 0)
			continue;

		for (uint8_t destination = 0; destination < 64; destination++)
		{
			if (moves & (bitboard)1 << destination & ~board->getOccupied(board->turn))
			{
				legalMoves.push_back({current, destination});
			}
		}
	}

	return legalMoves;
}

bitboard MoveGeneration::getPawnMoves(uint8_t position)
{
	bitboard enemyOccupied = this->board->getOccupied(1 - this->board->turn);
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;

	// Standard moveByChar
	if(board->turn == White)
	{
		result |= (piecePosition << 7) & enemyOccupied;	
		result |= (piecePosition << 8) & ~enemyOccupied;	
		result |= (piecePosition << 9) & enemyOccupied;	
	}
	else 
	{
		result |= (piecePosition >> 7) & enemyOccupied;	
		result |= (piecePosition >> 8) & ~enemyOccupied;
		result |= (piecePosition >> 9) & enemyOccupied;	
	}

	// Double moveByChar
	if (piecePosition & kStartPawn & kPieceColor[board->turn])
	{
		if(board->turn == White)
			result |= (piecePosition << 16) & ~enemyOccupied;
		else
			result |= (piecePosition >> 16) & ~enemyOccupied;
	}

	return result;
}

bitboard MoveGeneration::getKnightMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;

	result |= (piecePosition << 17) & kNotAFile;
	result |= (piecePosition << 10) & kNotABFile;
	result |= (piecePosition >> 6) & kNotABFile;
	result |= (piecePosition >> 15) & kNotAFile;
	result |= (piecePosition << 15) & kNotHFile;
	result |= (piecePosition << 6) & kNotGHFile;
	result |= (piecePosition >> 10) & kNotGHFile;
	result |= (piecePosition >> 17) & kNotHFile;
	return result;
}

bitboard MoveGeneration::getBishopMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;
	result |= northEastOccluded(piecePosition);
	result |= southEastOccluded(piecePosition);
	result |= northWestOccluded(piecePosition);
	result |= southWestOccluded(piecePosition);
	return result;
}

bitboard MoveGeneration::getRookMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;
	result |= northOccluded(piecePosition);
	result |= eastOccluded(piecePosition);
	result |= southOccluded(piecePosition);
	result |= westOccluded(piecePosition);
	return result;
}

bitboard MoveGeneration::getQueenMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;
	result |= northEastOccluded(piecePosition);
	result |= southEastOccluded(piecePosition);
	result |= northWestOccluded(piecePosition);
	result |= southWestOccluded(piecePosition);
	result |= northOccluded(piecePosition);
	result |= eastOccluded(piecePosition);
	result |= southOccluded(piecePosition);
	result |= westOccluded(piecePosition);
	return result;
}

bitboard MoveGeneration::getKingMoves(uint8_t position)
{
	bitboard piecePosition = (bitboard)1 << position;
	bitboard result = 0;

	result |= piecePosition << 8;
	result |= (piecePosition << 9) & kNotAFile;
	result |= (piecePosition << 1) & kNotAFile;
	result |= (piecePosition >> 7) & kNotAFile;
	result |= piecePosition >> 8;
	result |= (piecePosition >> 9) & kNotHFile;
	result |= (piecePosition >> 1) & kNotHFile;
	result |= (piecePosition << 7) & kNotHFile;

	return result;
}

bitboard MoveGeneration::northOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	board |= empty & ((board & enemyUnoccupied) << 8);
	return board;
}

bitboard MoveGeneration::eastOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	board |= empty & ((board & enemyUnoccupied) << 1);
	return board;
}

bitboard MoveGeneration::southOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn);
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	board |= empty & ((board & enemyUnoccupied) >> 8);
	return board;
}

bitboard MoveGeneration::westOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	board |= empty & ((board & enemyUnoccupied) >> 1);
	return board;
}

bitboard MoveGeneration::northEastOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) << 9);
	board |= empty & ((board & enemyUnoccupied) << 9);
	board |= empty & ((board & enemyUnoccupied) << 9);
	board |= empty & ((board & enemyUnoccupied) << 9);
	board |= empty & ((board & enemyUnoccupied) << 9);
	board |= empty & ((board & enemyUnoccupied) << 9);
	board |= empty & ((board & enemyUnoccupied) << 9);
	return board;
}

bitboard MoveGeneration::southEastOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotAFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) >> 7);
	board |= empty & ((board & enemyUnoccupied) >> 7);
	board |= empty & ((board & enemyUnoccupied) >> 7);
	board |= empty & ((board & enemyUnoccupied) >> 7);
	board |= empty & ((board & enemyUnoccupied) >> 7);
	board |= empty & ((board & enemyUnoccupied) >> 7);
	board |= empty & ((board & enemyUnoccupied) >> 7);
	return board;
}

bitboard MoveGeneration::northWestOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) << 7);
	board |= empty & ((board & enemyUnoccupied) << 7);
	board |= empty & ((board & enemyUnoccupied) << 7);
	board |= empty & ((board & enemyUnoccupied) << 7);
	board |= empty & ((board & enemyUnoccupied) << 7);
	board |= empty & ((board & enemyUnoccupied) << 7);
	board |= empty & ((board & enemyUnoccupied) << 7);
	return board;
}

bitboard MoveGeneration::southWestOccluded(bitboard board)
{
	bitboard empty = ~this->board->getOccupied(this->board->turn) & kNotHFile;
	bitboard enemyUnoccupied = ~this->board->getOccupied(1 - this->board->turn);
	board |= empty & ((board & enemyUnoccupied) >> 9);
	board |= empty & ((board & enemyUnoccupied) >> 9);
	board |= empty & ((board & enemyUnoccupied) >> 9);
	board |= empty & ((board & enemyUnoccupied) >> 9);
	board |= empty & ((board & enemyUnoccupied) >> 9);
	board |= empty & ((board & enemyUnoccupied) >> 9);
	board |= empty & ((board & enemyUnoccupied) >> 9);
	return board;
}

// Function to determine if a square is in check
bool MoveGeneration::isInCheck(int square)
{
	//Check if is king
	if (board->pieces[0][King] & (bitboard)1 << square)
	{
		int8_t opponentColor = 1 - board->turn;

		// Pretend the king is a piece and see if it can capture an identical piece of the opposite color
		if (getPawnMoves(square) & board->pieces[opponentColor][Pawn])
			return true;
		if (getKnightMoves(square) & board->pieces[opponentColor][Knight])
			return true;
		if (getRookMoves(square) & board->pieces[opponentColor][Rook])
			return true;
	}
	return false;
}

// Function to return a 64-bit integer, where an en passant capture could occur, for a particular square
bitboard MoveGeneration::getEnPassant(int square)
{
	// Ensure that the previous moveByChar was a pawn advance of 2 squares
	move prevMove = board->moves.top();

	bitboard pawns = board->pieces[White][Pawn] | board->pieces[Black][Pawn];

	if (absDiff(prevMove.startPosition / 8, prevMove.targetPosition / 8) == 2 &&	// Check if opposite moveByChar was length of 2
		(pawns & (bitboard)prevMove.targetPosition) &&									// Check if a opposite pawn was moved
		(square / 8 == prevMove.targetPosition / 8) &&									// Check if pawn is on the same row
		absDiff(square % 8, prevMove.targetPosition % 8) == 1)						// Check if pawn is next to opposite pawn
	{
		// For white pawns - ensure the king isn't in check
		if(prevMove.targetPosition / 8 == 3) // Check if on white en passant row
			return (bitboard)1 << prevMove.targetPosition - 8;
		else 
			return (bitboard)1 << prevMove.targetPosition + 8;
	}
	return 0;
}


// Function to return a 64-bit integer of all the castling moves
bitboard MoveGeneration::getCastlingMoves(uint8_t square)
{
	bitboard moves = 0;

	// White king castling
	if (square == 60)
	{
		// Make sure that there are no blocking pieces to the queenside
		if (board->wQueenSide && !(board->getAllPieces() & 112)) //0111 0000‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[White][King] &= 48; //0011 0000
			board->updateCombinedBitboard();

			// Ensure that the king does not castle through check
			if (!isInCheck(58) && !isInCheck(59) && !isInCheck(60))
				moves &= 32; // 0010 0000

			board->pieces[White][King] &= ~48; //0011 0000

			board->updateCombinedBitboard();
		}

		// Make sure that there are no blocking pieces to the kingside
		if (board->wKingSide && !(board->getAllPieces() & 6)) //0000 0110‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[White][King] &= 6; //0000 0110‬
			board->updateCombinedBitboard();

			// Ensure that the king does not castle through check
			if (!isInCheck(60) && !isInCheck(61) && !isInCheck(62))
				moves &= 2; // 00010
		}
	}
		// Black king castling
	else if (square == 4)
	{
		// Make sure that there are no blocking pieces to the queenside
		if (board->bQueenSide && !(board->getAllPieces() & 8070450532247928832)) //0111 0000‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[Black][King] &= 3458764513820540928; //0011 0000
			board->updateCombinedBitboard();

			// Ensure that the king does not castle through check
			if (!isInCheck(2) && !isInCheck(3) && !isInCheck(4))
				moves &= 2305843009213693952; // 0010 0000

			board->pieces[Black][King] &= ~3458764513820540928; //0011 0000

			board->updateCombinedBitboard();
		}

		// Make sure that there are no blocking pieces to the kingside
		if (board->bKingSide && !(board->getAllPieces() & 432345564227567616)) //0110 0000 ‬
		{
			// Pretend that there are kings at the squares where it castles through
			board->pieces[Black][King] &= 432345564227567616; // 0110‬ 0000
			board->updateCombinedBitboard();

			// Ensure that the king does not castle through check
			if (!isInCheck(4) && !isInCheck(5) && !isInCheck(6))
				moves &= 144115188075855872; // 0010 0000
		}
	}

	return moves;
}

int MoveGeneration::perft(int depth)
{
		bitboard nodes = 0;

		if(depth == 0)
			return 1;

		auto move_list = getAllMoves();
		for(int i = 0; i < move_list.size(); i++)
		{
			board->doMove(move_list[i]);
			int n = board->pieces[board->turn][King];
			if(!isInCheck(log2(n & -n) + 1))
				nodes += perft(depth - 1);
			board->undoMove(move_list[i]);
		}
		return nodes;
	
}
