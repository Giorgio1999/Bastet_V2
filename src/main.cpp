#include "Board.h"
#include "UciHandler.h"
#include "EngineController.h"
#include <iostream>
#include <map>
#include <cstring>

void Bench();

int main(int argc, char *argv[]) {
	if(argc == 2){
		std::string command = argv[1];
		if(command == "bench"){
			Bench();
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