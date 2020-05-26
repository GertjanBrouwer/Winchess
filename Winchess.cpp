#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "Board.h"
#include "MoveGeneration.h"
#include "UCI.h"
#include "Evaluation.h"

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
		total++;
		if (total < 900)
		{
			success++;
			continue;
		}
		auto fen = line.substr(0, line.find(" d1 "));
		board->setBoard(fen);

		auto expectedMoves = line.erase(0, line.find(" d4 ") + 4);
		expectedMoves = expectedMoves.substr(0, expectedMoves.find(" ;"));

		int foundMoves = mg->perft(4);

		if (foundMoves == std::stoi(expectedMoves))
			success++;
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
	board->doMove({54, 0});
	auto foundMoves = generation->getAllMoves();

	board->printBitboard();

	//TestDepth1(board, generation);

	Evaluation* eval = new Evaluation(board);

	std::cout << eval->getBoardValue() << std::endl;

	double duration;
	auto start = std::clock();
	int moves = generation->perft(2);

	std::cout << "Amount of found moves at depth 5: " << moves << std::endl;
	std::cout << "Captures: " << board->captures << std::endl;
	std::cout << "Castles: " << board->castles << std::endl;
	std::cout << "Promotions: " << board->promotions << std::endl;
	std::cout << "En passant: " << board->enpassants << std::endl;

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
