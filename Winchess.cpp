#include <iostream>
#include "UCI.h"
#include "Board.h"
#include <bitset>

int main()
{
	Board* board = new Board();

	UCI* uci = new UCI(board);

	uci->Read();

}