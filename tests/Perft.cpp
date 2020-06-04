#include "Perft.h"

#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <limits.h>
#	include <unistd.h>
#	include <string>
#endif


#include "../src/Board.h"
#include "../src/MoveGeneration.h"



std::string getexepath()
{
	#ifdef _WIN32
	char result[MAX_PATH];
	auto executable = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
	#else
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	auto executable = std::string(result, (count > 0) ? count : 0);
	#endif

	return executable.substr(0, executable.find_last_of("\\/"));
}

bool Perft::Test()
{

	std::cout << " - Starting PERFT tests" << std::endl;
	Board* board = new Board();
	MoveGeneration* generation = new MoveGeneration(board);

	std::string line;
	std::ifstream myfile;

	std::string file = getexepath() + "/resources/perft-test.epd";
	const char* testFile = file.c_str();

	std::cout << " - opened file: "<< testFile << std::endl;
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
