#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "Board.h"
#include "Evaluation.h"
#include "MoveGeneration.h"
#include "Search.h"
#include "UCI.h"

void TestDepth1(Board* board, MoveGeneration* mg)
{
	std::string line;
	std::ifstream myfile;
	myfile.open("C:\\Users\\Henrico\\Downloads\\marcel\\perft-test.epd");

	auto total = 0;
	auto success = 0;

	if(!myfile.is_open())
	{
		perror("Error open");
		exit(EXIT_FAILURE);
	}
	while(getline(myfile, line))
	{
		total++;
		if(total < 900)
		{
			success++;
			continue;
		}
		auto fen = line.substr(0, line.find(" d1 "));
		board->setBoard(fen);

		auto expectedMoves = line.erase(0, line.find(" d4 ") + 4);
		expectedMoves = expectedMoves.substr(0, expectedMoves.find(" ;"));

		int foundMoves = mg->perft(4);

		if(foundMoves == std::stoi(expectedMoves))
			success++;
		else
			std::cout << "Failed | Expected: " << expectedMoves << " - Found: " << foundMoves << " | " << fen << std::endl;
	}

	std::cout << "Total: " << total << std::endl;
	std::cout << "Success: " << success << std::endl;
}

int main()
{
	Board* board = new Board();

	board->setBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	MoveGeneration* generation = new MoveGeneration(board);

	UCI* uci = new UCI(board);

	//int depth = 2;
	//while(true)
	//{
	//	auto start = std::clock();
	//	auto bestmove = Search::findBestMove(board, depth, Black);
	//
	//	depth++;
	//}

	uci->Read();
}

int test(int depth)
{
	if(depth <= 0)
		return 1;
	return test(depth - 1);
}
