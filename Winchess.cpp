#include <iostream>
#include "Board.h"
#include "MoveGeneration.h"

int main()
{
	Board board;
	MoveGeneration* generation = new MoveGeneration(&board);
	board.move("b2b6");
	generation->getAllMoves();
	board.printBitboard();
}