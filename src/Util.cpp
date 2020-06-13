#include "Util.h"

short Util::bitIndex(bitboard board)
{
#if __GNUC__
	return __builtin_ctzll(board);
#elif __INTEL_COMPILER
	return _bit_scan_forward(board);
#elif _WIN32
	unsigned long pieceIndex;
	_BitScanForward64(&pieceIndex, board);
	return pieceIndex;
#else
	return ffsl(board);
#endif
}

short Util::bitCount(bitboard board)
{
#if __GNUC__
	return __builtin_popcountll(board);
#elif _WIN32
	return __popcnt64(board);
#else
	return __builtin_popcountll(board);
#endif
}

std::string Util::getExePath()
{
#ifdef _WIN32
	char result[MAX_PATH];
	auto executable = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
#else
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	auto executable = std::string(result, (count > 0) ? count : 0);
#endif

	return executable.substr(0, executable.find_last_of("\\/"));
}