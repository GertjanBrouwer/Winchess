#pragma once
#include <iostream>
#include "Board.h"
class UCI
{
public:

	UCI(Board* bitboard);
	~UCI();


	void Read();

	void inputUCI();
	void inputSetOptions(std::string inputstring);
	void inputIsReady();
	void inputUCINewGame();
	void inputPosition(std::string inputString);
	void inputGo();
	void inputStop();
	void inputPrint();
	void getFen();
	void inputQuit();

private:
	std::string engineName;
	Board* board;
	char command[256];
	std::string charToString();
	std::string concatString(std::string input, int length);
};
