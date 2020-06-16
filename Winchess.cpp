#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>

#include "src/Board.h"
#include "src/MoveGeneration.h"
#include "src/UCI.h"
#include "src/TranspositionTable.h"

int main()
{
	TranspositionTable::globalInstance = new TranspositionTable();
	Board* board = new Board();
	UCI* uci = new UCI(board);
	uci->Read();
}
