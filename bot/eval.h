#pragma once

#include "node.h"

struct WeightAttack {
	int clear[4] = { 0, 0, 0, 0 };
	int tspin[3] = { 0, 0, 0 };
	int perfect_clear = 0;
	int waste_time = 0;
	int waste_T = 0;
	int waste_I = 0;
	int b2b = 0;
	int ren = 0;
};

struct WeightDefence {
	int max_height = 0;
	int max_height_top_half = 0;
	int max_height_top_quarter = 0;
	int bumpiness = 0;
	int bumpiness_s = 0;
	int flat = 0;
	int transition_row = 0;
	int transition_column = 0;
	int hole = 0;
	int hole_s = 0;
    int crack = 0;
    int crack_s = 0;
	int blocked = 0;
	int blocked_s = 0;
	int well[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
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
	void evaluate(Node& node, PieceType* queue, int queue_count);
public:
	static int well(BitBoard& board, int column_height[10], int& well_position);
	static void bumpiness(int column_height[10], int well_position, int well_depth, int result[3]);
	static int transition_row(BitBoard& board, int column_height[10]);
	static int transition_column(BitBoard& board, int column_height[10]);
	static int hole(BitBoard& board, int column_height[10]);
	static int crack(BitBoard& board, int column_height[10], int well_position, int min_height);
	static void blocked(BitBoard& board, int column_height[10], int blocked[2]);
	static PieceData structure(BitBoard& board, int column_height[10]);
	static bool quiescence(BitBoard& board, int column_height[10], int depth, int tspin_structure[4]);
	static int spike(Node& root, Node& node);
};

constexpr Weight DEFAULT_WEIGHT() 
{
	Weight result;

    result.attack.clear[0] = -18000;
	result.attack.clear[1] = -14000;
	result.attack.clear[2] = -8500;
	result.attack.clear[3] = 39000;
	result.attack.tspin[0] = 12100;
	result.attack.tspin[1] = 41000;
	result.attack.tspin[2] = 68200;
	result.attack.perfect_clear = 10000000;
	result.attack.waste_time = -13000;
	result.attack.waste_T = -15200;
	result.attack.waste_I = 0;
	result.attack.b2b = 10500;
	result.attack.ren = 17500;

	result.defence.max_height = -3500;
	result.defence.max_height_top_half = -15000;
	result.defence.max_height_top_quarter = -60000;
	result.defence.bumpiness = -2400;
	result.defence.bumpiness_s = -700;
	result.defence.flat = 2000;
	result.defence.transition_row = -500;
	result.defence.transition_column = -500;
	result.defence.hole = -17300;
	result.defence.hole_s = -300;
	result.defence.crack = -15000;
	result.defence.crack_s = -300;
	result.defence.blocked = -1700;
	result.defence.blocked_s = -100;
	result.defence.well[0] = 3500;
	result.defence.well[1] = 4000;
	result.defence.well[2] = 4500;
	result.defence.well[3] = 5000;
	result.defence.well[4] = 5100;
	result.defence.well[5] = 4500;
	result.defence.well[6] = 5300;
	result.defence.well[7] = 5700;
	result.defence.well[8] = 4100;
	result.defence.well[9] = 3500;
	result.defence.structure[0] = 800;
	result.defence.structure[1] = 10000;
	result.defence.structure[2] = 15000;
	result.defence.structure[3] = 40700;
	result.defence.b2b = 5200;

    return result;
};