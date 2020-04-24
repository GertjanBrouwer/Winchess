﻿#include <iostream>
#include "Board.h"
#include "MoveGeneration.h"

int main()
{
	Board board;
	MoveGeneration* generation = new MoveGeneration(&board);
	generation->GetAllMoves(0);
	board.printBitboard();
}