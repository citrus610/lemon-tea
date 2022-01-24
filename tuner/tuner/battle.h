#pragma once

#include "player.h"

class Battle
{
public:
    Battle();
public:
    std::vector<LemonTea::PieceType> bag;
public:
    Player player_1;
    Player player_2;
public:
    LemonTea::Search bot_1;
    LemonTea::Search bot_2;
    std::vector<LemonTea::MoveType> movement_1;
    std::vector<LemonTea::MoveType> movement_2;
public:
    void init();
    void update();
    void update_player(Player& player, LemonTea::Search& bot, std::vector<LemonTea::MoveType>& movement);
    void render();
public:
    bool is_gameover();
    int get_winner();
};