#pragma once
#include <iostream>
#include "Board.h"
class UCI
{
public:
	UCI(Board* bitboard);
	~UCI();

	void Start();

private:
	void inputUCI();
	void inputSetOptions();
	void inputIsReady();
	void inputUCINewGame();
	void inputPosition();
	void inputGo();
	void inputStop();
	void inputPrint();
	void getFen();
	void inputQuit();

	std::string engineName;
	Board* board;
	char command[2048];
};
