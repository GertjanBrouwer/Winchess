#include <bitset>
#include <iostream>
#include "UCI.h"
#include <ctime>
#include "Board.h"
#include "MoveGeneration.h"

int main()
{
	Board* board = new Board();

	MoveGeneration* generation = new MoveGeneration(board);

	UCI* uci = new UCI(board);


	
	board->setBoard("rnkq1bnr/pppp1pbp/1p6/B2Pp3/8/2K5/PPP1PPPP/RNBQ2NR w - e6 0 1");

	
	board->printBitboard();
	auto moves = generation->getAllMoves();


	std::cout << "Amount of found moves: " << moves.size() << std::endl;

	uci->Read();
}
