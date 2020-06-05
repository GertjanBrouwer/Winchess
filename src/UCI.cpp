#include "UCI.h"

#include <algorithm>
#include <atomic>
#include <iostream>
#include <thread>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>

#include "Converter.h"
#include "Evaluation.h"
#include "Search.h"
#include "TranspositionTable.h"

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
		std::cin.getline(command, 2047);
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
	std::cout << "id name " + engineName << std::endl;
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
			if(moveLength < 0)
				moveLength = cmd.length();
			std::cout << cmd.substr(0, moveLength) << std::endl;

			board->moveByChar(cmd.substr(0, moveLength).c_str());

			//remove the first move in the list
			cmd = cmd.erase(0, moveLength + 1);
		}
	}
	std::cout << "info string " << board->getFen() << std::endl;
};

void search(Board* board)
{
	Search::ai_thread_running.exchange(true);
	TranspositionTable::globalInstance->clear();

	int depth = 2;

	MoveGeneration* moveGenerator = new MoveGeneration(board);
	Move bestMove = moveGenerator->getAllMoves()[0];
	delete moveGenerator;

	while(Search::ai_thread_running)
	{
		std::cout << "info depth " << depth << std::endl;

		Move foundMove = Search::findBestMove(board, depth, Black);
		if(foundMove.startPosition == -1)
			// Ignore found move if smaller than 0
			break;

		bestMove = foundMove;
		depth++;
		std::cout << "info currmove " << Converter::formatMove(bestMove) << " currmovenumber " << depth - 1 << std::endl;
	}


	std::cout << "bestmove " << Converter::formatMove(bestMove) << std::endl;
}

std::string getTime(std::string command, std::string part)
{
	if(command.find(part) != std::string::npos)
	{
		auto test = part.length();
		command.erase(0, command.find(part) + part.length() + 1);
		return command.substr(0, command.find(' '));
	}

	return "0";
}

int calculateEvalTime(Board* board)
{
	// Heuristics from http://http://facta.junis.ni.ac.rs/acar/acar200901/acar2009-07.pdf
	int materialScore = Evaluation::GetPieceBasedEvaluationOfColor(board, board->turn);
	if(materialScore < 20)
		return materialScore + 10;
	if(20 <= materialScore && materialScore <= 60)
		return round((3 / 8 * float(materialScore))) + 22;
	return round((3 / 8 * float(materialScore))) - 30;
}

void timeClock(int timeLeft, int increment, Board* board)
{ 
	auto startTime = std::chrono::steady_clock::now();
	int searchTime = std::min((int)((timeLeft / calculateEvalTime(board) + increment) * 0.9),
							 timeLeft - 100);

	std::cout << "info string give search time: " << searchTime << "ms" << std::endl;
	while(true)
	{
		auto now = std::chrono::steady_clock::now();
		int calculatedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
		if(calculatedTime >= searchTime)
		{
			// Exit the search thread and return best move found
			std::cout << "info string ai thread cancelled (calculatedTime: " << calculatedTime << ")" << std::endl;
			Search::ai_thread_running.exchange(false);
			return;
		}
	}
}

void UCI::inputGo()
{
	std::string cmd = command;
	int timeLeft;
	int increment;
	if(board->turn == White)
	{
		timeLeft = std::stoi(getTime(cmd, "wtime"));
		increment = std::stoi(getTime(cmd, "winc"));
	}
	else
	{
		timeLeft = std::stoi(getTime(cmd, "btime"));
		increment = std::stoi(getTime(cmd, "binc"));
	}

	
	Search::ai_thread_running.exchange(true);

	std::thread ai_thread = std::thread(search, board);
	ai_thread.detach();

	if(timeLeft > 0)
	{
		std::thread timer_thread = std::thread(timeClock, timeLeft, increment, board);
		timer_thread.detach();
	}
};

void UCI::inputStop()
{
	Search::ai_thread_running.exchange(false);
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
