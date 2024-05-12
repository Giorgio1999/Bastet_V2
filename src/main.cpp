#include "Board.h"
#include "UciHandler.h"
#include "EngineController.h"
#include <iostream>
#include <map>
#include <cstring>

void Bench();
void Fullperft();
void Validate();

int main(int argc, char *argv[]) {
	if(argc == 2){
		std::string command = argv[1];
		if(command == "bench"){
			Bench();
		}
		if(command == "fullperft"){
			Fullperft();
		}
		if(command == "validate"){
			Validate();
		}
	}
	else{
		Listen();
	}
	return 0;
}

void Bench(){
	EngineController engineController = EngineController();
	engineController.Bench();
}

void Fullperft(){
	EngineController engineController = EngineController();
	engineController.FullPerftTest();
}

void Validate(){
	EngineController engineController = EngineController();
	engineController.Validate();
}