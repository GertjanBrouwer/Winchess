#include <bitset>
#include <iostream>
#include "UCI.h"
#include "Board.h"

int main()
{
	Board* board = new Board();

	UCI* uci = new UCI(board);

	uci->Read();

}