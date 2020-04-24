#include <bitset>
#include <iostream>
#include "UCI.h"
#include "Board.h"
#include <bitset>

int main()
{
	Board* board = new Board();
	
	MoveGeneration* generation = new MoveGeneration(&board);
	//board.moveByChar("b2b6");
	
	UCI* uci = new UCI(board);

	uci->Read();
	
	
	board.printBitboard();

	for (auto i = 0; i < 10; i++)
	{
		std::cout << " " << i <<" ply | " << generation->perft(i) << std::endl;
	}
}
