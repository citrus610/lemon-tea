#include "eval.h"

int eval::evaluate(node& node)
{
	int score = 0;

	// Get column heights
	int column_height[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	node.board.get_height(column_height);

	// Max height
	int max_height = *std::max_element(column_height, column_height + 10);
	score += max_height * heuristic.max_height;
	score += std::max(max_height - 10, 0) * heuristic.max_height_top_half;
	score += std::max(max_height - 15, 0) * heuristic.max_height_top_quarter;

	// Well
	int well_index = -1;
	int well_depth = eval::well(node.board, column_height, well_index);
	well_depth = std::min(well_depth, 15);
	score += well_depth * heuristic.well;
	if (well_index == 0 || well_index == 9)
		score += well_depth * heuristic.well_index[0]; // Well 1 - 10
	else if (well_index == 1 || well_index == 8)
		score += well_depth * heuristic.well_index[1]; // Well 2 - 8
	else if (well_index == 2 || well_index == 7)
		score += well_depth * heuristic.well_index[2]; // Well 3 - 7
	else
		score += well_depth * heuristic.well_index[3]; // Well center

	// Bumpiness
	int bumpiness[3] = { 0, 0, 0 };
	eval::bumpiness(column_height, well_index, bumpiness);
	score += bumpiness[0] * heuristic.bumpiness;
	score += bumpiness[1] * heuristic.bumpiness_s;
	score += bumpiness[2] * heuristic.bumpiness_t;

	// Blocked cells
	int blocked_cell = eval::blocked_cell(node.board, column_height);
	score += blocked_cell * heuristic.blocked_cell;
	score += blocked_cell * blocked_cell * heuristic.blocked_cell_s;

	// Hole
	int hole = eval::hole(node.board, column_height);
	score += hole * heuristic.hole;
	score += hole * hole * heuristic.hole_s;

	// Block above hole
	int block_above_hole[2] = { 0, 0 };
	eval::block_above_hole(node.board, column_height, block_above_hole);
	score += block_above_hole[0] * heuristic.block_above_hole;
	score += block_above_hole[1] * heuristic.block_above_hole_s;

	// Structure
	eval::structure(node.board, column_height, node.structure);
	if (node.lock != LOCK_TSPIN_1 && node.lock != LOCK_TSPIN_2 && node.lock != LOCK_TSPIN_3) {
		node.waste_structure[0] += std::min(0, node.pre_structure[0] - node.structure[0]);
		node.waste_structure[1] += std::min(0, node.pre_structure[1] - node.structure[1]);
	}
	score += node.structure[0] * heuristic.structure[0];
	score += node.structure[1] * heuristic.structure[1];
	score += node.waste_structure[0] * heuristic.waste_structure[0];
	score += node.waste_structure[1] * heuristic.waste_structure[1];

	// B2B
	score += node.b2b * heuristic.b2b_chain;
	score += node.max_b2b * heuristic.b2b_max_chain;

	// REN
	score += node.ren * heuristic.ren_chain;
	score += node.max_ren * heuristic.ren_max_chain;

	// Perfect clear
	score += node.pc * heuristic.perfect_clear;

	// T spin
	score += node.tspin[0] * heuristic.t_spin[0]; // T spin single
	score += node.tspin[1] * heuristic.t_spin[1]; // T spin double
	score += node.tspin[2] * heuristic.t_spin[2]; // T spin triple

	// Clear lines
	score += node.clear[0] * heuristic.clear[0]; // Burn 1 line
	score += node.clear[1] * heuristic.clear[1]; // Burn 2 lines
	score += node.clear[2] * heuristic.clear[2]; // Burn 3 lines
	score += node.clear[3] * heuristic.clear[3]; // Tetris

	// Waste pieces
	score += node.waste_I * heuristic.waste_I;
	score += node.waste_T * heuristic.waste_T;

	// Waste time
	if (node.is_soft_srop && node.lock != LOCK_TSPIN_1 && node.lock != LOCK_TSPIN_2 && node.lock != LOCK_TSPIN_3 && node.lock != LOCK_PC)
		++node.waste_time;
	if (node.lock == LOCK_CLEAR_1 || node.lock == LOCK_CLEAR_2 || node.lock == LOCK_CLEAR_3)
		++node.waste_time;
	score += node.waste_time * heuristic.waste_time;

	return score;
}

int eval::well(bitboard& board, int column_height[10], int& well_index)
{
	// Find well index - index of lowest column
	int *min_height_ptr = std::min_element(column_height, column_height + 10);
	int min_height = *min_height_ptr;
	well_index = (int)(min_height_ptr - column_height);

	// Find well depth
	uint64_t mask = ~0b0;
	for (int i = 0; i < 10; ++i) {
		if (i == well_index) continue;
		mask = mask & board.column[i];
	}
	mask = mask << min_height;
	return std::countr_one(mask);
}

void eval::bumpiness(int column_height[10], int well_index, int result[3])
{
	result[0] = 0; result[1] = 0; result[2] = 0;
	for (int i = 0; i < 9; ++i) {
		if (i == well_index - 1 || i == well_index) continue;
		result[0] += std::abs(column_height[i] - column_height[i + 1]);
		result[1] += (column_height[i] - column_height[i + 1]) * (column_height[i] - column_height[i + 1]);
		result[2] += (column_height[i] != column_height[i + 1]);
		//}
		//else if (i == well_index - 1 && well_index != 9) {
		//	result[0] += std::abs(column_height[i] - column_height[i + 2]);
		//	result[1] += (column_height[i] - column_height[i + 2]) * (column_height[i] - column_height[i + 2]);
		//	result[2] += (column_height[i] != column_height[i + 2]);
		//}
	}
	if (well_index != 0 && well_index != 9) {
		result[0] += std::abs(column_height[well_index - 1] - column_height[well_index + 1]);
		result[1] += (column_height[well_index - 1] - column_height[well_index + 1]) * (column_height[well_index - 1] - column_height[well_index + 1]);
		result[2] += (column_height[well_index - 1] != column_height[well_index + 1]);
	}
}

int eval::blocked_cell(bitboard& board, int column_height[10])
{
	int result = 0;
	for (int i = 0; i < 10; ++i) {
		result += column_height[i] - std::popcount(board.column[i]);
	}
	return result;
}

/*
* A hole is an empty cell which is fully blocked up, left and right
*/
int eval::hole(bitboard& board, int column_height[10])
{
	int result = 0;
	for (int i = 0; i < 10; ++i) {
		// If there aren't any holes, then skip
		if ((board.column[i] & (board.column[i] + 1)) == 0) continue;

		// Count holes
		int acc_l_zero_and_l_one = 0;
		while (true)
		{
			uint64_t column = board.column[i] << acc_l_zero_and_l_one;
			int column_l_zero_cnt = std::countl_zero(column);
			acc_l_zero_and_l_one += column_l_zero_cnt + std::countl_one(column << column_l_zero_cnt);
			if (63 - acc_l_zero_and_l_one < 0) break;
			if (board.is_occupied(i - 1, 63 - acc_l_zero_and_l_one) && board.is_occupied(i + 1, 63 - acc_l_zero_and_l_one))
				++result;
		}

	}
	return result;
}

void eval::block_above_hole(bitboard& board, int column_height[10], int result[2])
{
	result[0] = 0; result[1] = 0;
	for (int i = 0; i < 10; ++i) {
		// If there aren't any holes, then skip
		if ((board.column[i] & (board.column[i] + 1)) == 0) continue;

		// shift column << (64 - column_height)
		// reverse bit ~
		// block above hole = count lead zero
		int block_above_hole_count = std::countl_zero(~(board.column[i] << (64 - column_height[i])));

		result[0] += block_above_hole_count;
		result[1] += block_above_hole_count * block_above_hole_count;
	}
}

void eval::structure(bitboard& board, int column_height[10], int result[2])
{
	result[0] = 0; result[1] = 0;
	for (int _x = 0; _x < 8; ++_x) {
		// If column height somewhat resemble t spin structure, then check
		// If check structure true, then sweet!
		// Else move on

		// TSD
		if (column_height[_x + 0] > column_height[_x + 1] && column_height[_x + 0] + 1 < column_height[_x + 2]) {
			if (((board.column[_x + 0] >> (column_height[_x + 0] - 1)) & 0b111) == 0b001 &&
				((board.column[_x + 1] >> (column_height[_x + 0] - 1)) & 0b111) == 0b000 &&
				((board.column[_x + 2] >> (column_height[_x + 0] - 1)) & 0b111) == 0b101) {
				++result[0];
				_x += 2;
			}
		}
		else
		if (column_height[_x + 2] > column_height[_x + 1] && column_height[_x + 2] + 1 < column_height[_x + 0]) {
			if (((board.column[_x + 0] >> (column_height[_x + 2] - 1)) & 0b111) == 0b101 &&
				((board.column[_x + 1] >> (column_height[_x + 2] - 1)) & 0b111) == 0b000 &&
				((board.column[_x + 2] >> (column_height[_x + 2] - 1)) & 0b111) == 0b001) {
				++result[0];
				_x += 2;
			}
		}
		// TST or STSD
		else
		if (column_height[_x + 1] >= column_height[_x + 0] && column_height[_x + 1] + 1 < column_height[_x + 2]) {
			if (((board.column[_x + 0] >> (column_height[_x + 1] - 3)) & 0b11000) == 0b00000 &&
				((board.column[_x + 1] >> (column_height[_x + 1] - 3)) & 0b11110) == 0b00100 &&
				((board.column[_x + 2] >> (column_height[_x + 1] - 3)) & 0b11111) == 0b10000) {
				++result[1];
				_x += 2;
			}
		}
		else
		if (column_height[_x + 1] >= column_height[_x + 2] && column_height[_x + 1] + 1 < column_height[_x + 0]) {
			if (((board.column[_x + 0] >> (column_height[_x + 1] - 3)) & 0b11111) == 0b10000 &&
				((board.column[_x + 1] >> (column_height[_x + 1] - 3)) & 0b11110) == 0b00100 &&
				((board.column[_x + 2] >> (column_height[_x + 1] - 3)) & 0b11000) == 0b00000) {
				++result[1];
				_x += 2;
			}
		}
	}
}

/*
* Set the weight to standard set
*/
void weight::standard()
{
	max_height = -39;
	max_height_top_half = -150;
	max_height_top_quarter = -611;
	bumpiness = -24;
	bumpiness_s = -7;
	bumpiness_t = -13;
	hole = -173;
	hole_s = -3;
	blocked_cell = -127;
	blocked_cell_s = -1;
	block_above_hole = -17;
	block_above_hole_s = -1;
	well = 70;
	well_index[0] = 20;
	well_index[1] = 20;
	well_index[2] = 24;
	well_index[3] = 60;
	structure[0] = 8;
	structure[1] = 148;
	waste_structure[0] = -152;
	waste_structure[1] = -152;
	b2b_chain = 104;
	b2b_max_chain = 52;
	ren_chain = 12;
	ren_max_chain = 150;
	clear[0] = -173;
	clear[1] = -122;
	clear[2] = -88;
	clear[3] = 390;
	t_spin[0] = 121;
	t_spin[1] = 410;
	t_spin[2] = 682;
	perfect_clear = 999;
	waste_time = -162;
	waste_T = -152;
	waste_I = -97;
}
