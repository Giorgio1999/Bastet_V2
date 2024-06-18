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
			MathUtility::Random<int> prng((int)2938472947865982);
			std::string path = argv[2];
			std::cout << path;
			std::fstream out(path, std::ios::out);
			while (true)
			{
				out << prng.Next(-100,100) << std::endl;
			}
			out.close();
		}
		if(command == "timertest"){
			Timer timer(0,0);
			std::string dummy = "";
			std::cout << "key+enter to stop" << std::endl;
			std::cin >> dummy;
			auto elapsed = timer.TimeElapsed();
			std::cout << "Time elapsed: " << elapsed << "ms" << std::endl;
		}
		if(command=="sorttest"){
			std::array<std::string,8> target = {"minus-eins","sechs","minus-vier","drei","minus-zwei","eins","-","-"};
			std::array<int,8> comparer = {-1,6,-4,3,-2,1,238523,245478};
			std::cout << "Target ";
			for(uint i=0;i<8;i++){
				std::cout << target[i] << " ";
			}
			std::cout << std::endl;

			MathUtility::Sort<std::string,int,8>(target,comparer,6,true);
			std::cout << "Result ";
			for(uint i=0;i<8;i++){
				std::cout << target[i] << " ";
			}
			std::cout << std::endl;
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