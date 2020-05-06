#include <bitset>
#include <iostream>
#include "UCI.h"

#include "Board.h"

int main()
{
	Board* board = new Board();
	
	UCI* uci = new UCI(board);

	uci->Read();
	
	

	MoveGeneration* generation = new MoveGeneration(board);

	board->printBitboard();
	auto moves = generation->getAllMoves();


	std::cout << "Amount of found moves: " << moves.size() << std::endl;
}
