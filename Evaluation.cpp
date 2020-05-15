#include "Evaluation.h"
#include <iostream>
#include <intrin.h>
#include <sstream>

Evaluation::Evaluation(Board* board) 
{
	this->board = board;
}

Evaluation::~Evaluation() 
{

}

int Evaluation::PopulationCount()
{
	int total = 0;
	unsigned int whiteTotal = 0;
	unsigned int blackTotal = 0;

	//count total bits in board white
	for(int type = 0; type < 5; type++)
	{
		whiteTotal += __popcnt64(board->pieces[White][type]) * typeValue.find(type)->second;
	}

	//count total bits in board black
	for(int type = 0; type < 5; type++)
	{
		blackTotal += __popcnt64(board->pieces[Black][type]) * typeValue.find(type)->second;
	}

	//evaluate count and determine
	total += whiteTotal;
	total -= blackTotal;

	//return count
	return total;
}

std::string Evaluation::getBoardValue() 
{
	std::stringstream ss;
	int value = PopulationCount();
	std::string color = (value > 0) ? "White " : (value == 0) ? "Even board " : "Black ";
	ss << value;
	return color + ss.str();
}
