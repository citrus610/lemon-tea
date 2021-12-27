#include <iostream>
#include "tuner.h"

using namespace std;

int main()
{
    srand((unsigned int)time(NULL));

	Tuner tuner;
	tuner.start();

    // Battle battle;
	// battle.init();

	// int frame = 0;

	// auto start = chrono::steady_clock::now();
	// while (!battle.is_gameover())
	// {
	// 	++frame;
	// 	system("CLS");
	// 	battle.update();
	// 	battle.render();
	// 	this_thread::sleep_for(chrono::milliseconds(10));
	// }
	// auto end = chrono::steady_clock::now();
	// cout << "TIME: " << ((double)chrono::duration_cast<chrono::milliseconds>(end - start).count() / 1000) << " s" << endl;
	// cout << "GAMEOVER... PLAYER " << battle.get_winner() << " WIN!" << endl;
	// cout << "    PLAYER 1 SENT " << battle.player_1.line_sent << endl;
	// cout << "    PLAYER 2 SENT " << battle.player_2.line_sent << endl;
	// cout << "FRAME " << frame << endl;
    return 0;
}