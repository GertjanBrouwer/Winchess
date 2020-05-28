#include "UCI.h"

#include <atomic>
#include <iostream>
#include <thread>

#include "Converter.h"
#include "Search.h"

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
	while (true)
	{
		//read incomming line and save in char array command
		std::cin.getline(command, 2048);
		std::cout << "-----------------" << std::endl;

		if (strstr(command, "setoption"))
			inputSetOptions();
		else if (strstr(command, "isready"))
			inputIsReady();
		else if (strstr(command, "ucinewgame"))
			inputUCINewGame();
		else if (strstr(command, "position"))
			inputPosition();
		else if (strstr(command, "go"))
			inputGo();
		else if (strstr(command, "stop"))
			inputStop();
		else if (strstr(command, "print"))
			inputPrint();
		else if (strstr(command, "uci"))
			inputUCI();
		else if (strstr(command, "getfen"))
			getFen();
		else if (strstr(command, "quit"))
			inputQuit();
		else
			std::cout << "non valid command '" << command << "'" << std::endl;
		std::cout << "-----------------" << std::endl;
	}
};

void UCI::inputUCI()
{
	std::cout << "id name" + engineName << std::endl;
	std::cout << "id author team Winchess" << std::endl;
	//options go here
	std::cout << "uciok" << std::endl;
};

void UCI::inputSetOptions()
{
	std::cout << "NOT IMPLEMENTED YET" << std::endl;
};

void UCI::inputIsReady()
{
	std::cout << "readyok" << std::endl;
};

void UCI::inputUCINewGame()
{
	std::cout << "starting new game..." << std::endl;
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	board->setBoard(fen);
};

//Used to set the board to a specific fen and set moves within that board state
void UCI::inputPosition()
{
	std::string cmd = command;
	//remove posistion_ from input
	cmd = cmd.erase(0, 9);

	if (cmd.find("startpos") != std::string::npos)
	{
		//remove startpos_ from input
		cmd = cmd.erase(0, 9);

		std::cout << "found startpos" << std::endl;

		//set bitboard to startpos
		board->setBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	}
	else
	{
		std::cout << "found fen" << std::endl;

		std::string fen = cmd.erase(0, cmd.find("fen ") + 4);
		//set bitboard to fen
		board->setBoard(fen);
		cmd = cmd.erase(0, cmd.substr(0, cmd.find(" moves")).length());
	}
	if (cmd.find("moves") != std::string::npos)
	{
		//remove moves_ from input
		cmd = cmd.erase(0, cmd.find("moves") + 6);

		while (cmd.length() > 0)
		{
			int moveLength = cmd.find(' ');
			if (moveLength < 0)
				moveLength = cmd.length();
			std::cout << cmd.substr(0, moveLength) << std::endl;

			board->moveByChar(cmd.substr(0, moveLength).c_str());

			//remove the first move in the list
			cmd = cmd.erase(0, moveLength + 1);
		}
	}
	std::cout << "info string " << board->getFen() << std::endl;
};

std::atomic<bool> ai_thread_running{false};
std::atomic<Move> globalBestMove{{}};
std::thread ai_thread;

void search(Board* board)
{
	const clock_t begin_time = clock();
	int depth = 2;
	Move bestMove;
	while (float(clock() - begin_time) / CLOCKS_PER_SEC < 2)
	{
		std::cout << "info depth " << depth << std::endl;

		bestMove = Search::findBestMove(board, depth, Black);
		depth++;
		globalBestMove.exchange(bestMove);
		std::cout << "info currmove " << Converter::formatMove(bestMove) << " currmovenumber " << depth - 1 << std::
			endl;
	}
	std::cout << "bestmove " << Converter::formatMove(bestMove) << std::endl;
}

void UCI::inputGo()
{
	ai_thread_running.exchange(false);
	ai_thread_running.exchange(true);
	auto thread = std::thread(search, board);
	thread.detach();
};

void UCI::inputStop()
{
	std::cout << "bestmove " << Converter::formatMove(globalBestMove) << std::endl;
}

void UCI::inputQuit()
{
	exit(0);
	std::cout << "NOT IMPLEMENTED YET" << std::endl;
}

void UCI::getFen()
{
	std::cout << board->getFen() << std::endl;
};

void UCI::inputPrint()
{
	board->printBitboard();
};
