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

    result.attack.clear[0] = -19041;
	result.attack.clear[1] = -14495;
	result.attack.clear[2] = -9145;
	result.attack.clear[3] = 39568;
	result.attack.tspin[0] = 11050;
	result.attack.tspin[1] = 40264;
	result.attack.tspin[2] = 68192;
	result.attack.perfect_clear = 10000000;
	result.attack.waste_time = -13081;
	result.attack.waste_T = -14287;
	result.attack.waste_I = -147;
	result.attack.b2b = 9975;
	result.attack.ren = 18709;

	result.defence.max_height = -3543;
	result.defence.max_height_top_half = -14934;
	result.defence.max_height_top_quarter = -60486;
	result.defence.bumpiness = -2738;
	result.defence.bumpiness_s = -719;
	result.defence.flat = 2878;
	result.defence.transition_row = -471;
	result.defence.transition_column = -479;
	result.defence.hole = -18747;
	result.defence.hole_s = -292;
	result.defence.crack = -18162;
	result.defence.crack_s = -317;
	result.defence.blocked = -3292;
	result.defence.blocked_s = -76;
	result.defence.well[0] = 4700;
	result.defence.well[1] = 5200;
	result.defence.well[2] = 5500;
	result.defence.well[3] = 5700;
	result.defence.well[4] = 5100;
	result.defence.well[5] = 5500;
	result.defence.well[6] = 5300;
	result.defence.well[7] = 5700;
	result.defence.well[8] = 5200;
	result.defence.well[9] = 4600;
	result.defence.structure[0] = -997;
	result.defence.structure[1] = 9987;
	result.defence.structure[2] = 13976;
	result.defence.structure[3] = 39662;
	result.defence.b2b = 5104;

    return result;
};