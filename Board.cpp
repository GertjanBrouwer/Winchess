#include "Board.h"
#include <map>
#include <sstream>

Board::Board()
{
	for(uint8_t color = 0; color < 2; color++)
	{
		for(uint8_t type = 0; type < 6; type++)
		{
			// kStartPiecePositions = bitboards of start position of all the pieces
			// kPieceColor = bitboards of the chess piece colors
			// When doing a bit and-operation on these 2 bitboards. Only the places where both bitboards have a 1 will remain a 1.
			// This will in the case of rooks and white. Only leave a bit on the first and 8th position in the resulting bitboard.
			pieces[color][type] = kStartPiecePositions[type] & kPieceColor[color];
		}
	}
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
void Board::move(const char* move)
{
	uint8_t startPosition = positionToIndex(move);
	uint8_t targetPosition = positionToIndex(&move[2]);
	piece movePiece = getPieceAt(startPosition);
	if(movePiece.color < 0)
		return;
	pieces[movePiece.color][movePiece.type] =
		(pieces[movePiece.color][movePiece.type] - ((bitboard)1 << startPosition)) + ((bitboard)1 << targetPosition);
}

uint8_t Board::positionToIndex(const char* position)
{
	// Calculate index using ASCII values (a = 97 and 1 = 49)
	// b : 98 - 97 = 1
	// 2 : (50 - 49) * 8 = 8
	// b2 results in 8 + 1 = 9
	return position[0] - 97 + (position[1] - 49) * 8;
}

piece Board::getPieceAt(uint8_t index)
{
	for(uint8_t color = 0; color < 2; color++)
	{
		for(uint8_t type = 0; type < 6; type++)
		{
			// Move bit at index position to front and check if piece is present (bit and-operation)
			if((pieces[color][type] >> index) & 0b1)
			{
				return {color, type};
			}
		}
	}

	return {-1, -1};
}

void Board::clearBoard()
{
	for(uint8_t color = 0; color < 2; color++)
	{
		for(uint8_t type = 0; type < 6; type++)
		{
			pieces[color][type] = 0;
		}
	}
}

void Board::setBoard(std::string fen)
{
	clearBoard();

	int currentPosition = -1;
	int row, column, cPos;
	std::string fenBoard = fen.substr(0, fen.find(' '));
	fen = fen.erase(0, fenBoard.length() + 1);

	for(uint8_t index = 0; index < fenBoard.length(); index++)
	{
		char currentPiece = fenBoard[index];

		if(isdigit(currentPiece))
		{
			//convert current int into the ascii table
			currentPosition += (int)currentPiece - 48;
		}
		if(isalpha(currentPiece))
		{
			currentPosition += 1;
			short type;

			short color = 1;

			if(isupper(currentPiece))
			{
				color = 0;
				currentPiece = tolower(currentPiece);
			}

			type = types.find(currentPiece)->second; 

			row = currentPosition / 8;
			column = currentPosition % 8;
			cPos = (7 - row) * 8 + column;

			//bitshift with OR opperator using a bitboard of only one piece
			pieces[color][type] |= (bitboard)1 << cPos;
		}
	}

	//check if next char w of b for turn
	//concat 1char + space
	activeColor = (fen.substr(0, fen.find(' ')) == "w") ? 'w' : 'b';
	fen = fen.erase(0, 2);

	//check if possible to castle
	//concat length of string + space
	std::string castlemove = fen.substr(0, fen.find(' '));
	whiteKingCastle = (castlemove.find('K') != std::string::npos) ? true : false;
	whiteQueenCastle = (castlemove.find('Q') != std::string::npos) ? true : false;
	blackKingCastle = (castlemove.find('k') != std::string::npos) ? true : false;
	blackQueenCastle = (castlemove.find('q') != std::string::npos) ? true : false;
	fen = fen.erase(0, castlemove.length() + 1);

	//check if en passant is possible
	//concat length of string + space
	enPassant = fen.substr(0, fen.find(' '));
	fen = fen.erase(0, enPassant.length() + 1);

	//save number of halfmoves in attribute
	//concat number + space
	std::string hmc = fen.substr(0, fen.find(' '));
	halfmoveClock = std::atoi(hmc.c_str());
	fen = fen.erase(0, hmc.length() + 1);

	//save number of fullmoves in attribute
	//concat number
	FullmoveNumber = std::atoi(fen.c_str());
}

std::string Board::getFen()
{
	std::string result;
	int counter = 0;
	int index = 0;

	for(uint8_t row = 8; row > 0; row--)
	{
		for(uint8_t col = 0; col < 8; col++)
		{
			index = row * 8 + col - 8;
			piece piece = getPieceAt(index);

			if(!(piece.color == -1 || piece.type == -1))
			{
				if(counter > 0)
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
		if(counter > 0)
		{
			result += intToString(counter);
			counter = 0;
		}
		if(row != 1)
		{
			result += "/";
		}
	}
	
	result += " ";
	result += activeColor;

	std::string castle = (whiteKingCastle == true) ? "K" : "";
	castle += (whiteQueenCastle == true) ? "Q" : "";
	castle += (blackKingCastle == true) ? "k" : "";
	castle += (blackQueenCastle == true) ? "q" : "";
	result += " "+castle;

	result += " " + enPassant;
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

	for(uint8_t index = 0; index < 64; index++)
	{
		piece piece = getPieceAt(index);
		if(piece.color >= 0)
		{
			result[index] = (piece.color == 0) ? toupper(kPieceChars[piece.type]) : kPieceChars[piece.type];
			continue;
		}
		result[index] = ' ';
	}

	std::cout << "\n|  A  B  C  D  E  F  G  H  |\n|8";
	for(short row = 7; row >= 0; row--)
	{
		for(short col = 0; col < 8; col++)
		{
			uint8_t index = row * 8 + col;
			std::cout << ' ' << result[index] << ' ';
			if((index + 1) % 8 == 0 && index + 1 != 8)
			{
				std::cout << row + 1 << "|\n"
						  << "|" << row;
			}
			else if(index + 1 != 8)
				std::cout << "";
		}
	}
	std::cout << "1|\n|  A  B  C  D  E  F  G  H  |\n";
}
