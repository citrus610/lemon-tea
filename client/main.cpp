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

    // {
    //     PieceType queue[16] = { PIECE_I, PIECE_J, PIECE_L, PIECE_S, PIECE_O, PIECE_Z, PIECE_T, PIECE_I, PIECE_J, PIECE_L, PIECE_S, PIECE_O, PIECE_Z, PIECE_T };
    //     int queue_cnt = 5;

    //     Tree tree;

    //     tree.init(queue, queue_cnt);

    //     auto t_start = chrono::high_resolution_clock::now();
    //     int n_cnt = tree.search(200);
    //     auto t_end = chrono::high_resolution_clock::now();

    //     cout << "init" << endl;
    //     cout << "node " << n_cnt << endl;
    //     cout << "time " << chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count() << endl;
    //     cout << "nps " << ((double)n_cnt / (double)chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count()) << "kn/s" << endl;
    // }
    // cout << "de" << endl;
    // cin.get();
    
    return 0;
};