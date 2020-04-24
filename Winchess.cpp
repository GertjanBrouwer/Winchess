#include <iostream>
#include "Board.h"
#include "MoveGeneration.h"

int main()
{
	Board board;
	MoveGeneration* generation = new MoveGeneration(&board);
	board.move("a1d4");
	generation->getAllMoves();
	board.printBitboard();
}