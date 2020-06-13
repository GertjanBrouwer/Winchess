#include <bitset>
#include <ctime>
#include <fstream>
#include <iostream>

#include "src/Util.h"
#include "src/Board.h"
#include "src/MoveGeneration.h"
#include "src/UCI.h"
#include "src/Model.h"
#include "src/Tensor.h"

int main()
{
	Board* board = new Board();
	board->setBoard("r2q1rk1/ppp2ppp/2np1n2/1Bb1p3/4P1b1/2N2N2/PPPP1PPP/R1BQR1K1 w Qq - 0 1");
	UCI* uci = new UCI(board);
	uci->Read();
}
