#include <bitset>
#include <iostream>
#include "Board.h"

int main()
{
	Board board;

	board.printBitboard();
	board.move("c2c4");
	board.printBitboard();
	board.move("c4c5");
	board.printBitboard();
	board.move("c5c6");
	board.printBitboard();
}