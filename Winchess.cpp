#include <bitset>
#include <iostream>
#include "UCI.h"
#include "Board.h"
#include <bitset>

int main()
{
	Board* board = new Board();
	
	MoveGeneration* generation = new MoveGeneration(&board);
	board.move("a1d4");
	
	UCI* uci = new UCI(board);

	uci->Read();
	
	
	board.printBitboard();
}