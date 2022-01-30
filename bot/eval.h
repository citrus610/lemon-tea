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
    int hole_a = 0;
    int hole_b = 0;
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
    static void hole(Board& board, int column_height[10], int min_height, int result[2]);
    static int blocked(Board& board, int column_height[10]);
    static Piece structure(Board& board, int column_height[10]);
    static void quiescence(Board& board, int column_height[10], int depth, int tspin_structure[4]);
    static int spike(Node& root, Node& node);
};

constexpr Weight DEFAULT_WEIGHT()
{
    Weight result;

    result.attack.clear[0] = -300;
    result.attack.clear[1] = -250;
    result.attack.clear[2] = -200;
    result.attack.clear[3] = 300;
    result.attack.tspin[0] = 50;
    result.attack.tspin[1] = 400;
    result.attack.tspin[2] = 700;
    result.attack.pc = 1500;
    result.attack.waste_time = -30;
    result.attack.waste_T = -150;
    result.attack.b2b = 105;
    result.attack.ren = 200;

    result.defence.height = -40;
    result.defence.height_10 = -150;
    result.defence.height_15 = -600;
    result.defence.bumpiness = -26;
    result.defence.bumpiness_s = -7;
    result.defence.flat = 14;
    result.defence.row_t = -5;
    result.defence.hole_a = -306;
    result.defence.hole_b = -293;
    result.defence.blocked = -32;
    result.defence.well = 35;
    result.defence.structure[0] = 100;
    result.defence.structure[1] = 150;
    result.defence.structure[2] = 200;
    result.defence.structure[3] = 500;
    result.defence.b2b = 62;

    return result;
};

};