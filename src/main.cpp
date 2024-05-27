#include "UciHandler.h"
#include "EngineController.h"
#include "MathUtility.h"
#include "BitBoardUtility.h"
#include <iostream>
#include <fstream>
#include <cstring>

// Functions for testing
// -------------------------------------------------------------------
void Bench();
void Fullperft();
void Validate();
// -------------------------------------------------------------------

// Main loop
// -------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// Execute command line arguments directly
	if (argc >= 2)
	{
		std::string command = argv[1];
		if (command == "-bench" || command == "bench")
		{
			Bench();
		}
		if (command == "fullperft")
		{
			Fullperft();
		}
		if (command == "validate")
		{
			Validate();
		}
		if (command == "prngtest")
		{
			MathUtility::Random<bitboard> prng((bitboard)2938472947865982);
			std::string path = argv[2];
			std::cout << path;
			std::fstream out(path, std::ios::out);
			while (true)
			{
				out << prng.Next() << std::endl;
			}
			out.close();
		}
	}
	else
	{
		// Fallback to UCI control
		Listen();
	}

	return 0;
}
// -------------------------------------------------------------------

// Functions for testing (implementation)
// -------------------------------------------------------------------
void Bench()
{
	EngineController engineController = EngineController();
	engineController.Bench();
}

void Fullperft()
{
	EngineController engineController = EngineController();
	engineController.FullPerftTest();
}
// -------------------------------------------------------------------

void Validate()
{
	EngineController engineController = EngineController();
	engineController.Validate();
}