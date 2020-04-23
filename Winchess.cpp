#include <iostream>
#include "Board.h"
#include <bitset>

int main()
{
	Board board;
	
	board.printBitboard();
	board.move("c2c4");
	board.printBitboard();
}