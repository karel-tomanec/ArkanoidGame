#include <exception>
#include <iostream>

#include "arkanoid.hpp"

int main(int argc, char* argv[])
{
	try
	{
		Arkanoid arkanoid;
		arkanoid.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';

	}

	return 0;
}