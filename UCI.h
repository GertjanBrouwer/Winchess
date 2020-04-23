#pragma once
#include <iostream>
class UCI
{
public:

	UCI();
	~UCI();


	void Read();

	void inputUCI();
	void inputSetOptions(std::string inputstring);
	void inputIsReady();
	void inputUCINewGame();
	void inputPosition(std::string inputString);
	void inputGo();
	void inputPrint();


private:
	std::string engineName;

	char command[256];
	std::string charToString();
	std::string concatString(std::string input, int length);
};
