#pragma once

#include "player.h"

class Battle
{
public:
	Battle();
public:
	std::vector<PieceType> bag;
public:
	Player player_1;
	Player player_2;
public:
	Tree bot_1;
	Tree bot_2;
	std::vector<MoveType> movement_1;
	std::vector<MoveType> movement_2;
public:
	void init();
	void update();
	void update_player(Player& player, Tree& bot, std::vector<MoveType>& movement);
	void render();
public:
	bool is_gameover();
	int get_winner();
};