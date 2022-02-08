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

    // LemonTea::Board board = LemonTea::Board();
    // std::vector<LemonTea::PathFinderNode> on_stack;
    // LemonTea::PathFinder::generate_onstack(board, LemonTea::PIECE_S, on_stack);

    // cout << "Position: " << int(on_stack.size()) << endl;
    // std::cin.get();
    // system("cls");

    // for (int i = 0; i < int(on_stack.size()); ++i) {
    //     LemonTea::Board copy = board;
    //     on_stack[i].placement.place(copy);
    //     draw_board(copy);
    //     cout << "Path: ";
    //     for (int m = 0; m < on_stack[i].path.get_size(); ++m) {
    //         cout << LemonTea::convert_move_to_string(on_stack[i].path[m]) << " ";
    //     }
    //     cout << endl;
    //     std::cin.get();
    //     system("cls");
    // }

    // LemonTea::Board board;
    // board[9] = 0b011111111111111111;
    // board[8] = 0b011111111111111111;
    // board[7] = 0b011111111111111111;
    // board[6] = 0b011111111111111111;
    // board[5] = 0b011111111111111111;
    // board[4] = 0b011111111111111111;
    // board[3] = 0b011111111111111111;
    // board[2] = 0b001111111111111111;
    // board[1] = 0b000111111111111111;
    // board[0] = 0b010111111111111111;

    // LemonTea::Piece destination = {
    //     .x = 1,
    //     .y = 15,
    //     .type = LemonTea::PIECE_S,
    //     .rotation = LemonTea::PIECE_UP
    // };

    // LemonTea::draw_path_finder(board, destination);

    return 0;
};