#include <iostream>
#include <windows.h>

#include "../bot/bot.h"
#include "tetris.h"

using namespace std;

int main()
{
    srand((unsigned int)time(NULL));

    TetrisGame::Tetris game(64, 32, 16);
    game.start();
    
    return 0;
};