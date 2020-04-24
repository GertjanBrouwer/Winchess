#include <iostream>
#include "Board.h"
#include "MoveGeneration.h"

int main()
{
	Board board;
	MoveGeneration* generation = new MoveGeneration(&board);
	//board.moveByChar("b2b6");
	generation->getAllMoves();
	board.printBitboard();

	for (auto i = 0; i < 10; i++)
	{
		std::cout << " " << i <<" ply | " << generation->perft(i) << std::endl;
	}
}
