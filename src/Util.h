#pragma once
#include "Board.h"
#ifdef _WIN32
//TODO: why is windows.h still needed when intrin.h has been included?
#	define NOMINMAX
#	include <intrin.h>
#	include <windows.h>
#else
#	include <limits.h>
#	include <unistd.h>
#	include <string>
#endif

class Util
{
public:
	static short bitIndex(bitboard board);
	static short bitCount(bitboard board);
	static std::string getExePath();
};