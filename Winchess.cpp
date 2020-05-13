#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "Board.h"
#include "MoveGeneration.h"
#include "UCI.h"

void TestDepth1(Board* board, MoveGeneration* mg)
{
	std::string line;
	std::ifstream myfile;
	myfile.open("C:\\Users\\Henrico\\Downloads\\marcel\\perft-test.epd");

	auto total = 0;
	auto success = 0;

	if (!myfile.is_open())
	{
		perror("Error open");
		exit(EXIT_FAILURE);
	}
	while (getline(myfile, line))
	{
		auto fen = line.substr(0, line.find(" d1 "));
		board->setBoard(fen);

		auto expectedMoves = line.erase(0, fen.length() + 4);

		expectedMoves = expectedMoves.substr(0, expectedMoves.find(" ;"));

		auto foundMoves = mg->getAllMoves().size();

		total++;

		if (foundMoves == std::stoi(expectedMoves))
		{
			success++;
			//std::cout << "Success | " << fen << std::endl;
		}
		else
			std::cout << "Failed | Expected: " << expectedMoves << " - Found: " << foundMoves << " | " << fen << std::
				endl;
	}


	std::cout << "Total: " << total << std::endl;
	std::cout << "Success: " << success << std::endl;
}

int main()
{
	Board* board = new Board();

	MoveGeneration* generation = new MoveGeneration(board);

	UCI* uci = new UCI(board);

	board->setBoard("b1k5/3n3p/8/R1Ppq3/P2r4/N2p3K/3P3n/2B5 w - - 0 1");

	board->printBitboard();
	auto moves = generation->getAllMoves();

	std::cout << "Amount of found moves: " << moves.size() << std::endl;

	TestDepth1(board, generation);

	uci->Read();
}
