#include "Board.h"
#include <iostream>
#include <map>
#include <sstream>

int Board::promotions = 0;
int Board::enpassants = 0;
int Board::castles = 0;
int Board::captures = 0;

Board::Board()
{
	for (int color = 0; color < 2; color++)
	{
		for (int type = 0; type < 6; type++)
		{
			// kStartPiecePositions = bitboards of start position of all the pieces
			// kPieceColor = bitboards of the chess piece colors
			// When doing a bit and-operation on these 2 bitboards. Only the places where both bitboards have a 1 will remain a 1.
			// This will in the case of rooks and white. Only leave a bit on the first and 8th position in the resulting bitboard.
			pieces[color][type] = kStartPiecePositions[type] & kPieceColor[color];
		}
	}
	this->updateBitboardCache();
}

Board::Board(Board* board)
{
	this->origin = board;
	std::copy(&board->pieces[0][0], &board->pieces[0][0] + 12, &this->pieces[0][0]);
	this->turn = board->turn;
	this->castleWQueenSide = board->castleWQueenSide;
	this->castleWKingSide = board->castleWKingSide;
	this->castleBQueenSide = board->castleBQueenSide;
	this->castleBKingSide = board->castleBKingSide;
	this->lastMove = board->lastMove;
	this->bitboardCache = board->bitboardCache;
}

// Moving the pawn e2->e4 from the initial chess position :
//
// pieces[white][pawns] bitboard looks like this : bits : 000000001111111100000000
//
// startPosition : bits : 000000000000100000000000
// targetPosition : bits : 000000000000000000001000
//
// now remove the startPosition bit from the bitboard,
// resulting in : 000000001111011100000000
// then add the targetPosition bit to the bitboard,
// resulting in : 000000001111011100001000
//
// When formatting into a board it looks like this : 00000000 11110111 00001000
void Board::moveByChar(const char* moveChar)
{
	short startPosition = positionToIndex(moveChar);
	short targetPosition = positionToIndex(&moveChar[2]);
	short promotionType = 0;
	if (strlen(moveChar) == 5)
		promotionType = types.at(moveChar[4]);

	doMove({startPosition, targetPosition, promotionType});
}

void Board::doMove(Move move)
{
	int from = move.startPosition;
	int to = move.targetPosition;
	Piece piece = getPieceAt(from);

	// Check if piece exists on location
	if (piece.color < 0)
		return;

	bitboard fromMask = (bitboard)1 << from;
	bitboard toMask = (bitboard)1 << to;

	// Captures
	for (int i = 0; i < 6; ++i)
	{
		if (pieces[White][i] & toMask || pieces[Black][i] & toMask)
			captures++;
	}

	pieces[White][Pawn] &= ~toMask;
	pieces[White][Knight] &= ~toMask;
	pieces[White][Bishop] &= ~toMask;
	pieces[White][Rook] &= ~toMask;
	pieces[White][Queen] &= ~toMask;
	pieces[White][King] &= ~toMask;

	pieces[Black][Pawn] &= ~toMask;
	pieces[Black][Knight] &= ~toMask;
	pieces[Black][Bishop] &= ~toMask;
	pieces[Black][Rook] &= ~toMask;
	pieces[Black][Queen] &= ~toMask;
	pieces[Black][King] &= ~toMask;

	// Castling
	if (piece.type == King && abs(from - to) == 2)
	{
		castles++;
		bitboard colorBitboard = piece.color == White ? kStartAllWhite : kStartAllBlack;

		if (from - to < 0)
		{
			//King side
			bitboard kingSideBitboard = 0b1000000000000000000000000000000000000000000000000000000010000000;
			bitboard movingRookBitboard = kingSideBitboard & colorBitboard;

			// Move rook and add then add the rooks that weren't on the start position
			pieces[piece.color][Rook] = movingRookBitboard >> 2 | (pieces[piece.color][Rook] & ~movingRookBitboard);
		}
		else
		{
			//Queen side
			bitboard queenSideBitboard = 0b0000000100000000000000000000000000000000000000000000000000000001;
			bitboard movingRookBitboard = queenSideBitboard & colorBitboard;

			// Move rook and add then add the rooks that weren't on the start position
			pieces[piece.color][Rook] = movingRookBitboard << 3 | (pieces[piece.color][Rook] & ~movingRookBitboard);
		}
	}

	if (toMask == 128)
		castleWKingSide = false;
	if (toMask == 1)
		castleWQueenSide = false;
	if (toMask == 0x8000000000000000)
		castleBKingSide = false;
	if (toMask == 0x100000000000000)
		castleBQueenSide = false;

	if (piece.type == King)
	{
		if (piece.color == White)
		{
			castleWKingSide = false;
			castleWQueenSide = false;
		}
		else
		{
			castleBKingSide = false;
			castleBQueenSide = false;
		}
	}

	if (piece.type == Rook)
	{
		if (from == 0 && piece.color == White)
			castleWQueenSide = false;
		if (from == 7 && piece.color == White)
			castleWKingSide = false;
		if (from == 56 && piece.color == Black)
			castleBQueenSide = false;
		if (from == 63 && piece.color == Black)
			castleBKingSide = false;
	}

	// En passant

	int diagMove = (to - from) % 2;
	if (piece.type == Pawn && (diagMove == 1 || diagMove == -1) && (getOccupied(1 - piece.color) & toMask) == 0)
	{
		enpassants++;
		if (piece.color == White)
			pieces[Black][Pawn] -= toMask >> 8;
		else
			pieces[White][Pawn] -= toMask << 8;
	}

	enPassant = 0;
	if (piece.type == Pawn && abs(to - from) == 16)
	{
		enPassant = (bitboard)1 << ((to + from) / 2);
	}

	// Promotions
	if (move.promotionPieceType != 0)
	{
		promotions++;
		pieces[piece.color][move.promotionPieceType] |= toMask;
		pieces[piece.color][piece.type] = pieces[piece.color][piece.type] - fromMask;
	}
	else
		pieces[piece.color][piece.type] = pieces[piece.color][piece.type] - fromMask + toMask;

	turn = static_cast<PieceColor>(1 - turn);

	this->updateBitboardCache();
}

Board* Board::getBoardWithMove(Move move)
{
	Board* newState = new Board(this);
	newState->lastMove = move;
	newState->doMove(move);
	return newState;
}

void Board::updateBitboardCache()
{
	bitboardCache.occupiedByColor[White] = 0;
	bitboardCache.occupiedByColor[Black] = 0;
	for (size_t index = 0; index < 6; index++)
	{
		bitboardCache.occupiedByColor[White] |= pieces[White][index];
		bitboardCache.occupiedByColor[Black] |= pieces[Black][index];
	}

	bitboardCache.occupied = bitboardCache.occupiedByColor[White] | bitboardCache.occupiedByColor[Black];
}

bitboard Board::getOccupied(int color)
{
	return bitboardCache.occupiedByColor[color];
}

// Returns a bitboard containing all the pieces on the board
bitboard Board::getAllPieces()
{
	return bitboardCache.occupied;
}

unsigned int Board::positionToIndex(const char* position)
{
	// Calculate index using ASCII values (a = 97 and 1 = 49)
	// b : 98 - 97 = 1
	// 2 : (50 - 49) * 8 = 8
	// b2 results in 8 + 1 = 9
	return position[0] - 97 + (position[1] - 49) * 8;
}

Piece Board::getPieceAt(int position)
{
	// @TODO Optimize with SIMD
	for (int color = 0; color < 2; color++)
	{
		for (int type = 0; type < 6; type++)
		{
			// Move bit at index position to front and check if piece is present (bit and-operation)
			if ((pieces[color][type] >> position) & 0b1)
			{
				return {color, type};
			}
		}
	}

	return {-1, -1};
}

void Board::clearBoard()
{
	for (uint8_t color = 0; color < 2; ++color)
	{
		for (uint8_t type = 0; type < 6; ++type)
		{
			pieces[color][type] = 0;
		}
	}
}

void Board::setBoard(std::string fen)
{
	clearBoard();

	int currentPosition = -1;
	int row, column, actualPosition;
	std::string fenBoard = fen.substr(0, fen.find(' '));
	fen = fen.erase(0, fenBoard.length() + 1);

	for (uint8_t index = 0; index < fenBoard.length(); ++index)
	{
		char currentPiece = fenBoard[index];

		if (isdigit(currentPiece))
		{
			//convert current int into the ascii table
			currentPosition += (int)currentPiece - 48;
		}
		if (isalpha(currentPiece))
		{
			currentPosition += 1;
			short type;

			short color = 1;

			if (isupper(currentPiece))
			{
				color = 0;
				currentPiece = tolower(currentPiece);
			}

			type = types.find(currentPiece)->second;

			row = currentPosition / 8;
			column = currentPosition % 8;
			actualPosition = (7 - row) * 8 + column;

			//bitshift with OR opperator using a bitboard of only one piece
			pieces[color][type] |= (bitboard)1 << actualPosition;
		}
	}

	//check if next char w or b for turn
	//concat 1char + space
	turn = (fen.substr(0, fen.find(' ')) == "w") ? White : Black;
	fen = fen.erase(0, 2);

	//check if possible to castle
	//concat length of string + space
	std::string castlemove = fen.substr(0, fen.find(' '));
	castleWKingSide = (castlemove.find('K') != std::string::npos) ? true : false;
	castleWQueenSide = (castlemove.find('Q') != std::string::npos) ? true : false;
	castleBKingSide = (castlemove.find('k') != std::string::npos) ? true : false;
	castleBQueenSide = (castlemove.find('q') != std::string::npos) ? true : false;
	fen = fen.erase(0, castlemove.length() + 1);

	//check if en passant is possible
	//concat length of string + space
	std::string enPassant = fen.substr(0, fen.find(' '));
	this->enPassant = 0;
	if (enPassant.size() == 2)
	{
		const char* enPassantChar = enPassant.c_str();
		this->enPassant = (bitboard)1 << positionToIndex(enPassantChar);
		fen = fen.erase(0, enPassant.length() + 1);
	}

	//save number of halfmoves in attribute
	//concat number + space
	std::string hmc = fen.substr(0, fen.find(' '));
	halfmoveClock = std::atoi(hmc.c_str());
	fen = fen.erase(0, hmc.length() + 1);

	//save number of fullmoves in attribute
	//concat number
	FullmoveNumber = std::atoi(fen.c_str());

	updateBitboardCache();
}

std::string Board::getFen()
{
	std::string result;
	int counter = 0;
	int index = 0;

	for (uint8_t row = 8; row > 0; row--)
	{
		for (uint8_t col = 0; col < 8; col++)
		{
			index = row * 8 + col - 8;
			Piece piece = getPieceAt(index);

			if (!(piece.color == -1 || piece.type == -1))
			{
				if (counter > 0)
				{
					//add empty spaces to fen
					result += intToString(counter);
					counter = 0;
				}
				//add piece to fen
				result += (piece.color == 0) ? toupper(kPieceChars[piece.type]) : kPieceChars[piece.type];
			}
			else
				counter++;
		}
		if (counter > 0)
		{
			result += intToString(counter);
			counter = 0;
		}
		if (row != 1)
		{
			result += "/";
		}
	}

	result += " ";
	result += turn == White ? 'w' : 'b';

	std::string castle = (castleWKingSide == true) ? "K" : "";
	castle += (castleWQueenSide == true) ? "Q" : "";
	castle += (castleBKingSide == true) ? "k" : "";
	castle += (castleBQueenSide == true) ? "q" : "";
	result += " " + castle;

	result += " -";
	result += " " + intToString(halfmoveClock);
	result += " " + intToString(FullmoveNumber);

	return result;
}

std::string Board::intToString(int& i)
{
	std::stringstream ss;
	ss << i;

	return ss.str();
}

void Board::printBitboard()
{
	char result[64];

	for (int index = 0; index < 64; index++)
	{
		Piece piece = getPieceAt(index);
		if (piece.color >= 0)
		{
			result[index] = (piece.color == 0) ? toupper(kPieceChars[piece.type]) : kPieceChars[piece.type];
			continue;
		}
		result[index] = ' ';
	}

	std::cout << "\n|  A  B  C  D  E  F  G  H  |\n|8";
	for (short row = 7; row >= 0; --row)
	{
		for (short col = 0; col < 8; ++col)
		{
			int index = row * 8 + col;
			std::cout << ' ' << result[index] << ' ';
			if ((index + 1) % 8 == 0 && index + 1 != 8)
			{
				std::cout << row + 1 << "|\n"
					<< "|" << row;
			}
			else if (index + 1 != 8)
				std::cout << "";
		}
	}
	std::cout << "1|\n|  A  B  C  D  E  F  G  H  |\n";
}
