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
	int column_transition = 0;
	int row_transition = 0;
	int hole = 0;
	int hole_s = 0;
	int crack = 0;
	int crack_s = 0;
	int blocked = 0;
	int blocked_s = 0;
	int well[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int structure[2] = { 0, 0 };
	int b2b = 0;
};

struct Weight {
	WeightAttack attack;
	WeightDefence defence;
};

class Evaluator
{
public:
	Weight weight;
public:
	void evaluate(Node& node, NodeState& node_state);
public:
	static int well(BitBoard& board, int column_height[10], int& well_position);
	static void bumpiness(int column_height[10], int well_position, int well_depth, int result[3]);
	static int column_transition(BitBoard& board, int column_height[10]);
	static int row_transition(BitBoard& board, int column_height[10]);
	static int hole(BitBoard& board, int column_height[10]);
	static int crack(BitBoard& board, int column_height[10], int well_position, int min_height);
	static void blocked(BitBoard& board, int column_height[10], int blocked[2]);
	static PieceData structure(BitBoard& board, int column_height[10]);
	static bool quiescence(BitBoard& board, int column_height[10], int depth, int tspin_structure[2]);
	static int spike(NodeState& root, Node& node, NodeState& node_state);
};

static Weight DEFAULT_WEIGHT()
{
	Weight result;

	result.attack.clear[0] = -143;
	result.attack.clear[1] = -100;
	result.attack.clear[2] = -56;
	result.attack.clear[3] = 390;
	result.attack.tspin[0] = 121;
	result.attack.tspin[1] = 410;
	result.attack.tspin[2] = 610;
	result.attack.perfect_clear = 1000;
	result.attack.waste_time = -120;
	result.attack.waste_T = -152;
	result.attack.waste_I = -35;
	result.attack.b2b = 100;
    result.attack.ren = 150;

	result.defence.max_height = -35;
	result.defence.max_height_top_half = -150;
	result.defence.max_height_top_quarter = -600;
	result.defence.bumpiness = -24;
	result.defence.bumpiness_s = -7;
	result.defence.flat = 15;
	result.defence.column_transition = -35;
	result.defence.row_transition = -5;
	result.defence.hole = -173;
	result.defence.hole_s = -3;
	result.defence.crack = -150;
	result.defence.crack_s = -1;
	result.defence.blocked = -17;
	result.defence.blocked_s = -1;
	result.defence.well[0] = 20;
	result.defence.well[1] = 30;
	result.defence.well[2] = 50;
	result.defence.well[3] = 60;
	result.defence.well[4] = 60;
	result.defence.well[5] = 60;
	result.defence.well[6] = 60;
	result.defence.well[7] = 50;
	result.defence.well[8] = 30;
	result.defence.well[9] = 20;
	result.defence.structure[0] = 500;
	result.defence.structure[1] = 400;
	result.defence.b2b = 100;

    return result;
};