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

	board->setBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	board->printBitboard();

	auto foundMoves = generation->getAllMoves();


	//TestDepth1(board, generation);

	double duration;
	auto start = std::clock();

	int moves = generation->perft(6);
	std::cout << "Amount of found moves at depth 6: " << moves << std::endl;

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	std::cout << "Duration: " << duration << '\n';
	uci->Read();
}

int test(int depth)
{
	if (depth <= 0)
		return 1;
	return test(depth - 1);
}
