#pragma warning (disable : 4996)
#include <iostream>
#include <vector>
#include <json/json.h>
#include <json/writer.h>
#include <fstream>
#include "GameHandler.h"

using namespace std;

int main()
{
	// main game controller, handles all functionality
	GameHandler gameHandler;

	// while user doesn't press 'n', a new game will start
	char anotherGame = 'y';
	while (anotherGame != 'n')
		anotherGame = gameHandler.GameCycle();

	return 0;
}