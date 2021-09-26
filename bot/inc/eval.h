#ifndef EVAL_H
#define EVAL_H

#include "node.h"

struct weight {
	int max_height = 0;
	int max_height_top_half = 0;
	int max_height_top_quarter = 0;
	int bumpiness = 0;
	int bumpiness_s = 0;
	int bumpiness_t = 0;
	int hole = 0;
	int hole_s = 0;
	int blocked_cell = 0;
	int blocked_cell_s = 0;
	int block_above_hole = 0;
	int block_above_hole_s = 0;
	int well = 0;
	int well_index[4] = { 0, 0, 0, 0 };
	int structure[2] = { 0, 0 };
	int waste_structure[2] = { 0, 0 };
	int b2b_chain = 0;
	int b2b_max_chain = 0;
	int ren_chain = 0;
	int ren_max_chain = 0;
	int ren_acc_chain = 0;
	int clear[4] = { 0, 0, 0, 0 };
	int t_spin[3] = { 0, 0, 0 };
	int perfect_clear = 0;
	int waste_time = 0;
	int waste_T = 0;
	int waste_I = 0;

	void standard();
};

class eval
{
public:
	weight heuristic;

public:
	int evaluate(node& node, piece_type* queue, int& queue_count);

public:
	static int well(bitboard& board, int column_height[10], int& well_index);
	static void bumpiness(int column_height[10], int well_index, int result[3]);
	static int blocked_cell(bitboard& board, int column_height[10]);
	static int hole(bitboard& board, int column_height[10]);
	static void block_above_hole(bitboard& board, int column_height[10], int result[2]);
	static void structure(bitboard& board, int column_height[10], int result[2]);
	static void quiescence(bitboard& board, int column_height[10], int depth_T);
};

#endif // EVAL_H