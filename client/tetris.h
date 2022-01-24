#pragma once

#include "game.h"
#include "tetris_battle.h"

namespace TetrisGame
{

static ConsoleEngine::Color piece_color(LemonTea::PieceType piece)
{
    switch (piece)
    {
    case LemonTea::PIECE_I:
        return ConsoleEngine::COLOR_FG_CYAN;
        break;
    case LemonTea::PIECE_J:
        return ConsoleEngine::COLOR_FG_BLUE;
        break;
    case LemonTea::PIECE_L:
        return ConsoleEngine::COLOR_FG_DARK_YELLOW;
        break;
    case LemonTea::PIECE_T:
        return ConsoleEngine::COLOR_FG_MAGENTA;
        break;
    case LemonTea::PIECE_S:
        return ConsoleEngine::COLOR_FG_GREEN;
        break;
    case LemonTea::PIECE_O:
        return ConsoleEngine::COLOR_FG_YELLOW;
        break;
    case LemonTea::PIECE_Z:
        return ConsoleEngine::COLOR_FG_RED;
        break;
    default:
        return ConsoleEngine::COLOR_FG_WHITE;
        break;
    }
};

class Tetris : public ConsoleEngine::Game
{
public:
    Tetris(int w, int h, int pixel_size) : ConsoleEngine::Game(w, h, pixel_size) {};
public:
    void load() override;
    void update(double dt) override;
    void render() override;
    void unload() override;
public:
    void render_battle(TetrisBattle& tetris_battle);
public:
    TetrisBattle battle;
};

static void create_json() 
{
    // Check if existed?
    std::ifstream f("config.json");
    if (f.good()) {
        return;
    };
    f.close();

    // Setting data
    json js;
    LemonTea::Weight heuristic = LemonTea::DEFAULT_WEIGHT();
    to_json(js, heuristic);

    // Create file
    std::ofstream o("config.json");
    o << std::setw(4) << js << std::endl;
    o.close();
};

static void set_from_json(json& js, LemonTea::Weight& heuristic) 
{
    from_json(js, heuristic);
};

};