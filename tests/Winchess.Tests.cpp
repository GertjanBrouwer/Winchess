#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "Perft.h"
#include "../src/MoveGeneration.h"

std::string exec(std::string cmd)
{
	std::array<char, 128> buffer;
	std::string result;
#ifdef _WIN32
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
#else
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
#endif
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	return result;
}

int main(int argc, char** argv)
{
	std::cout << "=== [STARTED WINCHESS TESTS] ===" << std::endl;

	bitboard board = (bitboard)1 << 8;

	std::cout << "8 | " << MoveGeneration::getBitIndex(board);

	Perft::Test();
}
