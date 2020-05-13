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
	Board* board2 = new Board(board);
	board2->printBitboard();

	Board* board3 = board->getBoardWithMove(Move{12, 20});
	board->printBitboard();

	//TestDepth1(board, generation);

	Evaluation* eval = new Evaluation(board);

	std::cout << eval->getBoardValue() << std::endl;

	uci->Read();
	double duration;
	start = std::clock();

	int moves = generation->perft(1);
	std::cout << "Amount of found moves: " << moves << std::endl;

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	std::cout << "Duration: " << duration << '\n';

	start = std::clock();
	for(auto i = 0; i < 19900000; i++)
	{
		board2 = new Board(board);
		delete board2;
	}
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "Duration loop: " << duration << '\n';

	do
	{
		std::cout << '\n' << "Press a key to continue...";
	} while(std::cin.get() != '\n');
}

int test(int depth)
{
	if(depth <= 0)
		return 1;
	return test(depth - 1);
}