#include <iostream>
#include "UCI.h"
#include "Board.h"
#include <bitset>

int main()
{
	UCI* uci = new UCI();

	uci->Read();
}
	Board board;
	
	board.printBitboard();
	board.move("c2c4");
	board.printBitboard();
	board.move("c4c5");
	board.printBitboard();
	board.move("c5c6");
	board.printBitboard();
}