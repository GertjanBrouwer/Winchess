#include "Perft.h"

#include <fstream>
#include <iostream>
#include <string>

#include "../src/Board.h"
#include "../src/MoveGeneration.h"

bool Perft::Test()
{

	std::cout << " - Starting PERFT tests" << std::endl;
	Board* board = new Board();
	MoveGeneration* generation = new MoveGeneration(board);

	std::string line;
	std::ifstream myfile;

	const char* testFile = "resources/perft-test.epd";
	std::cout << " - opening file" << std::endl;
	myfile.open(testFile);
	std::cout << " - opened file" << std::endl;

	auto total = 0;
	auto success = 0;

	if (!myfile.is_open())
	{
		std::cout << "Error opening file" << std::endl;
		perror("Error open");
		exit(EXIT_FAILURE);
	}
	while (getline(myfile, line))
	{
		total++;
		auto fen = line.substr(0, line.find(" d1 "));
		board->setBoard(fen);

		auto expectedMoves = line.erase(0, line.find(" d2 ") + 4);
		expectedMoves = expectedMoves.substr(0, expectedMoves.find(" ;"));
		
		int foundMoves = generation->perft(2);
		
		if(foundMoves == std::stoi(expectedMoves))
			success++;
		else
			std::cout << "::error file=tests/" << testFile<< ",line=" << total - 1 << ",col=0::Perf test failed! | Expected: " << expectedMoves << " - Found: " << foundMoves << " | " << fen << std::endl;
	}

	std::cout << "Total: " << total << std::endl;
	std::cout << "Success: " << success << std::endl;

	delete board;
	delete generation;

	return total == success;
}
