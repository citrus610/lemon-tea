#pragma once

#include "node.h"

struct WeightAttack {
	int clear[4] = { 0, 0, 0, 0 };
	int tspin[3] = { 0, 0, 0 };
	int perfect_clear = 0;
	int waste_time = 0;
	int waste_T = 0;
	int waste_I = 0;
};

struct WeightDefence {
	int max_height = 0;
	int max_height_top_half = 0;
	int max_height_top_quarter = 0;
	int bumpiness = 0;
	int bumpiness_s = 0;
	int bumpiness_t = 0;
	int row_transition = 0;
	int hole = 0;
	int hole_s = 0;
	int blocked = 0;
	int blocked_s = 0;
	int well = 0;
	int well_position[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int structure[2] = { 0, 0 };
	int b2b = 0;
	int ren = 0;
};

struct Weight {
	WeightAttack attack;
	WeightDefence defence;

	void standard();
};

class Evaluator
{
public:
	Weight weight;
public:
	void evaluate(Node& node, PieceType* queue, int& queue_count);
	void evaluate_forecast(NodeForecast& node_forecast);
public:
	static int well(BitBoard& board, int column_height[10], int& well_position);
	static void bumpiness(int column_height[10], int well_position, int well_depth, int result[3]);
	static int row_transition(BitBoard& board, int column_height[10]);
	static int hole(BitBoard& board, int column_height[10]);
	static void blocked(BitBoard& board, int column_height[10], int blocked[2]);
	static PieceData structure(BitBoard& board, int column_height[10]);
	static bool quiescence(BitBoard& board, int column_height[10], int depth, int tspin_structure[2]);
};