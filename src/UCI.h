#pragma once
#include <algorithm>
#include <atomic>
#include <iostream>
#include <thread>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>

#include "Board.h"
#include "Evaluation.h"
#include "MaterialEvaluation.h"
#include "NeuralNetEvaluation.h"
#include "Converter.h"
#include "Search.h"

class UCI
{
public:
	UCI(Board* bitboard);
	~UCI();

	void Read();
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

private:
	std::string engineName;
	Board* board;
	Evaluation* evaluator;
	char command[2048];
};
