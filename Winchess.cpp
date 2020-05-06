#include <iostream>
#include <thread>

#include "Board.h"
#include "MoveGeneration.h"
#include "UCI.h"

int main()
{
	Board* board = new Board();

	MoveGeneration* generation = new MoveGeneration(board);

	board->printBitboard();
	auto moves = generation->getAllMoves();


	std::cout << "Amount of found moves: " << moves.size() << std::endl;
}
