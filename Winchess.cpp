#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>

#include "src/Board.h"
#include "src/MoveGeneration.h"
#include "src/UCI.h"

int main()
{
	Board* board = new Board();
	UCI* uci = new UCI(board);
	uci->Read();
}
