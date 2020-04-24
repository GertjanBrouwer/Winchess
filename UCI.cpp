#include "UCI.h"
#include <iostream>

UCI::UCI(Board* bitboard)
{
	engineName = "Winchess v1";
	board = bitboard;
};

UCI::~UCI()
{

};

void UCI::Read()
{
	std::cout << "Give command" << std::endl;
	while(true)
	{
		//read incomming line and save in char array command
		std::cin.getline(command,256);
		std::cout << "-----------------" << std::endl;

		if (strstr(command, "setoption"))
		{
			inputSetOptions(charToString());
		}
		else if (strstr(command, "isready"))
		{
			inputIsReady();
		}
		else if (strstr(command, "ucinewgame"))
		{
			inputUCINewGame();
		}
		else if (strstr(command, "position"))
		{
			inputPosition(charToString());
		}
		else if (strstr(command, "go"))
		{
			inputGo();
		}
		else if (strstr(command, "print"))
		{
			inputPrint();
		}
		else if (strstr(command, "uci"))
		{
			inputUCI();
		}
		else if (strstr(command, "getfen"))
		{
			getFen();
		}
		else
		{
			std::cout << "non valid command" << std::endl;
		}
		std::cout << "-----------------" << std::endl;
	}
};

void UCI::inputUCI()
{
	std::cout << "id name"+ engineName << std::endl;
	std::cout << "id author team Winchess" << std::endl;
	//options go here
	std::cout << "uciok" << std::endl;
};

void UCI::inputSetOptions(std::string inputString)
{

};

void UCI::inputIsReady()
{
	std::cout << "readyok" << std::endl;
};

void UCI::inputUCINewGame()
{
	std::cout << "starting new game..." << std::endl;
	//const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	const char* fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR";
	board->setBoard(fen);
};

void UCI::inputPosition(std::string inputString)
{

	//remove posistion_ from input
	inputString = concatString(inputString,9);

	if(inputString.find("startpos") != std::string::npos)
	{
		//remove startpos_ from input
		inputString = concatString(inputString,9);

		std::cout << "found startpos" << std::endl;

		//set bitboard to startpos
	}
	else if (inputString.find("fen") != std::string::npos)
	{
		//remove fen_ from input
		inputString = concatString(inputString,4);
	
		std::cout << "found fen" << std::endl;

		//set bitboard to fen
	}
	if(inputString.find("moves") != std::string::npos)
	{
		//remove moves_ from input
		inputString = concatString(inputString,inputString.find("moves") + 6);

		while(inputString.length() > 0)
		{
			std::cout << inputString.substr(0, 4) << std::endl;

			board->move(inputString.substr(0,4).c_str());

			//remove the first move in the list
			inputString = concatString(inputString, 5);
		}
	}

};

void UCI::inputGo()
{

};

void UCI::getFen()
{
	std::cout << board->getFen() << std::endl;
};

void UCI::inputPrint()
{
	board->printBitboard();
};

std::string UCI::charToString()
{
	return std::string(command);
};

std::string UCI::concatString(std::string input, int length)
{
	return input.erase(0, length);
};