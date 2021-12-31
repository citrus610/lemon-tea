#pragma once

#include "tetris_player.h"

namespace TetrisGame
{

class TetrisBattle
{
public:
	TetrisBattle();
public:
	std::vector<PieceType> bag;
public:
	TetrisPlayer player_1;
	TetrisPlayer player_2;
public:
	Bot bot_1;
	Bot bot_2;
	Weight w_1;
	Weight w_2;
	std::vector<MoveType> movement_1;
	std::vector<MoveType> movement_2;
public:
	void init();
	void update();
	void update_player(TetrisPlayer& player, Bot& bot, std::vector<MoveType>& movement);
	void unload();
public:
	bool is_gameover();
	int get_winner();
};

}