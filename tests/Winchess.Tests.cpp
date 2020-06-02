#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "Perft.h"

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

	Perft::Test();


	const std::string engine1(argv[0]);
	const std::string engine2(argv[1]);

	exec("cutechess-cli -engine cmd=" + engine1 + " -engine cmd=" + engine2 + " -each proto=uci tc=40/2 -rounds 20");
}
