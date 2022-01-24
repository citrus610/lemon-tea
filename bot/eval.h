#pragma once

#include "node.h"

namespace LemonTea
{

struct WeightAttack {
    int clear[4] = { 0, 0, 0, 0 };
    int tspin[3] = { 0, 0, 0 };
    int pc = 0;
    int waste_time = 0;
    int waste_T = 0;
    int b2b = 0;
    int ren = 0;
};

struct WeightDefence {
    int height = 0;
    int height_10 = 0;
    int height_15 = 0;
    int bumpiness = 0;
    int bumpiness_s = 0;
    int flat = 0;
    int row_t = 0;
    int column_t = 0;
    int hole = 0;
    int crack = 0;
    int blocked = 0;
    int well = 0;
    int structure[4] = { 0, 0, 0, 0 };
    int b2b = 0;
};

struct Weight {
    WeightAttack attack = WeightAttack();
    WeightDefence defence = WeightDefence();
};

class Evaluator
{
public:
    Weight weight;
public:
    void evaluate(Node& node, PieceType* queue, int queue_size, PieceType* bag, int bag_size);
public:
    static int well(Board& board, int column_height[10], int& well_position);
    static void bumpiness(int column_height[10], int well_position, int result[3]);
    static int transition_row(Board& board, int column_height[10]);
    static int transition_column(Board& board, int column_height[10]);
    static void hole(Board& board, int column_height[10], int min_height, int result[2]);
    static void blocked(Board& board, int column_height[10], int blocked[2]);
    static Piece structure(Board& board, int column_height[10]);
    static void quiescence(Board& board, int column_height[10], int depth, int tspin_structure[4]);
    static int spike(Node& root, Node& node);
};

constexpr Weight DEFAULT_WEIGHT()
{
    Weight result;

    result.attack.clear[0] = -200;
    result.attack.clear[1] = -150;
    result.attack.clear[2] = -100;
    result.attack.clear[3] = 300;
    result.attack.tspin[0] = 50;
    result.attack.tspin[1] = 400;
    result.attack.tspin[2] = 700;
    result.attack.pc = 1500;
    result.attack.waste_time = -35;
    result.attack.waste_T = -150;
    result.attack.b2b = 101;
    result.attack.ren = 144;

    result.defence.height = -40;
    result.defence.height_10 = -150;
    result.defence.height_15 = -500;
    result.defence.bumpiness = -20;
    result.defence.bumpiness_s = -5;
    result.defence.flat = 20;
    result.defence.row_t = -58;
    result.defence.column_t = -57;
    result.defence.hole = -198;
    result.defence.crack = -100;
    result.defence.blocked = -24;
    result.defence.well = 25;
    result.defence.structure[0] = 100;
    result.defence.structure[1] = 150;
    result.defence.structure[2] = 200;
    result.defence.structure[3] = 500;
    result.defence.b2b = 81;

    return result;
};

};