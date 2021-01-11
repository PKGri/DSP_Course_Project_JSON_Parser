#include <iostream>

#include "Commands.h"

#include <regex>
#include <sstream>

int main()
{
	

	enterCommand(&isExit, &mainCommandList);
	
	

	std::cout << "Exiting";

	return 0;
}
