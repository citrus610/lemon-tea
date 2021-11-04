#include <iostream>
#include "bot.h"
#include "gamebot.h"

using namespace std;

int main()
{
	srand((unsigned int)time(NULL));

	gamebot new_game;
	new_game.start();

	//cin.get();
	return 0;
}