#include "Board.h" 

Board::Board()
{
	initBoard();
}

void Board::initBoard()
{
	for(short color = 0; color < 2; color++)
	{
		// Every side has only 6 unique piece types
		for(short type = 0; type < 6; type++)
		{
			pieces[color][type] = kStartPiecePositions[type] & kPieceColor[color];
		}
	}
}

void Board::setMove(const char* move) {
	short start = positionToIndex(move);
	short end = positionToIndex(&move[2]);
	piece startPiece = getPieceAt(start);
	pieces[startPiece.color][startPiece.type] = pieces[startPiece.color][startPiece.type] - ((bitboard)1 << start) + ((bitboard)1 << end);
}

short Board::positionToIndex(const char* position)
{
	return position[0] - 97 + (position[1] - 49) * 8;
}

piece Board::getPieceAt(short index)
{
	for(short color = 0; color < 2; color++)
	{
		for(short type = 0; type < 6; type++)
		{
			// Move bit at index position to front and check if piece is present (bit and-operation)
			if((pieces[color][type] >> index) & 0b1)
			{
				return {color, type};
			}
		}
	}

	return {-1,-1};
}
 
void Board::printBitboard()
{
	char result[64];

	for(short index = 0; index < 64; index++)
	{
		piece piece = getPieceAt(index);
		if(piece.color >= 0)
		{
			result[index] = (piece.color == 0) ? kPieceChars[piece.type] : toupper(kPieceChars[piece.type]);
			continue;
		}
		result[index] = ' ';
	}

	std::cout << "\n|  A  B  C  D  E  F  G  H  |\n|8";
	for(short row = 7; row >= 0; row--)
	{
		for(short col = 0; col < 8; col++)
		{
			short index = row * 8 + col;
			std::cout << ' ' << result[index] << ' ';
			if((index + 1) % 8 == 0 && index + 1 != 8)
			{
				std::cout << row + 1 << "|\n"  << "|" << row;
			}
			else if(index + 1 != 8) std::cout << "";
		}
	}
	std::cout << "1|\n|  A  B  C  D  E  F  G  H  |\n";
}