#include "eval.h"

void Weight::standard()
{
	attack.clear[0] = -200;
	attack.clear[1] = -150;
	attack.clear[2] = -100;
	attack.clear[3] = 400;
	attack.tspin[0] = 50;
	attack.tspin[1] = 400;
	attack.tspin[2] = 600;
	attack.perfect_clear = 1000;
	attack.ren = 100;
	attack.waste_time = -150;
	attack.waste_T = -150;
	attack.waste_I = -100;

	defence.max_height = -35;
	defence.max_height_top_half = -150;
	defence.max_height_top_quarter = -500;
	defence.bumpiness = -24;
	defence.bumpiness_s = -7;
	defence.bumpiness_t = -12;
	defence.hole = -200;
	defence.blocked = -35;
	defence.well = 55;
	defence.well_position[0] = 20;
	defence.well_position[1] = 20;
	defence.well_position[2] = 20;
	defence.well_position[3] = 40;
	defence.well_position[4] = 40;
	defence.well_position[5] = 40;
	defence.well_position[6] = 40;
	defence.well_position[7] = 20;
	defence.well_position[8] = 20;
	defence.well_position[9] = 20;
	defence.structure[0] = 8;
	defence.structure[1] = 100;
	defence.b2b = 100;
	defence.ren = 10;
}

void Evaluator::evaluate(Node& node, PieceType* queue, int& queue_count)
{
	/*
	* Defence evaluation
	*/
	node.score.defence = 0;
	BitBoard board = node.state.board;

	// Get column heights
	int column_height[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	board.get_height(column_height);

	//Structure
	PieceData t_struct = Evaluator::structure(board, column_height);
	if (t_struct.type != PIECE_NONE) {
		if (t_struct.rotation == PIECE_DOWN) {
			node.score.defence += this->weight.defence.structure[0];
		}
		else {
			node.score.defence += this->weight.defence.structure[1];
		}
	}

	// Height
	int max_height = *std::max_element(column_height, column_height + 10);
	node.score.defence += max_height * this->weight.defence.max_height;
	node.score.defence += std::max(max_height - 10, 0) * this->weight.defence.max_height_top_half;
	node.score.defence += std::max(max_height - 15, 0) * this->weight.defence.max_height_top_quarter;

	// Quiescence
	int quiescence_depth = (node.state.current == PIECE_T) + (node.state.hold == PIECE_T);
	for (int i = node.state.next; i < std::min(queue_count, node.state.next + 3); ++i) {
		quiescence_depth += queue[i] == PIECE_T;
	}
	bool quiescence = Evaluator::quiescence(board, column_height, quiescence_depth);

	// Well
	int well_position = 0;
	int well_depth = Evaluator::well(board, column_height, well_position);
	node.score.defence += well_depth * this->weight.defence.well;
	node.score.defence += well_depth * this->weight.defence.well_position[well_position];

	// Bumpiness
	int bumpiness[3];
	Evaluator::bumpiness(column_height, well_position, bumpiness);
	node.score.defence += bumpiness[0] * this->weight.defence.bumpiness;
	node.score.defence += bumpiness[1] * this->weight.defence.bumpiness_s;
	node.score.defence += bumpiness[2] * this->weight.defence.bumpiness_t;

	// Hole
	int hole = Evaluator::hole(board, column_height);
	node.score.defence += hole * this->weight.defence.hole;

	// Blocked hole
	int blocked = Evaluator::blocked(board, column_height);
	node.score.defence += blocked * this->weight.defence.blocked;

	// B2B
	node.score.defence += node.state.b2b * this->weight.defence.b2b;

	// Ren
	node.score.defence += node.state.ren * this->weight.defence.ren;


	/*
	* Attack evaluation
	*/
	// Attacks
	switch (node.action.lock)
	{
	case LOCK_CLEAR_1:
		node.score.attack += this->weight.attack.clear[0];
		break;
	case LOCK_CLEAR_2:
		node.score.attack += this->weight.attack.clear[1];
		break;
	case LOCK_CLEAR_3:
		node.score.attack += this->weight.attack.clear[2];
		break;
	case LOCK_CLEAR_4:
		node.score.attack += this->weight.attack.clear[3];
		break;
	case LOCK_TSPIN_1:
		node.score.attack += this->weight.attack.tspin[0];
		break;
	case LOCK_TSPIN_2:
		node.score.attack += this->weight.attack.tspin[1];
		break;
	case LOCK_TSPIN_3:
		node.score.attack += this->weight.attack.tspin[2];
		break;
	case LOCK_PC:
		node.score.attack += this->weight.attack.perfect_clear;
		break;;
	default:
		break;
	}

	// Ren
	node.score.attack += REN_LUT[std::min(node.state.ren, MAX_COMBO_TABLE_SIZE - 1)] * this->weight.attack.ren;

	// Waste time
	if (node.action.soft_drop &&
		node.action.lock != LOCK_TSPIN_1 &&
		node.action.lock != LOCK_TSPIN_2 &&
		node.action.lock != LOCK_TSPIN_3 &&
		node.action.lock != LOCK_PC)
		node.score.attack += this->weight.attack.waste_time;
	if (node.action.lock == LOCK_CLEAR_1 ||
		node.action.lock == LOCK_CLEAR_2 ||
		node.action.lock == LOCK_CLEAR_3)
		node.score.attack += this->weight.attack.waste_time;

	// Waste T
	if (node.action.placement.type == PIECE_T &&
		node.action.lock != LOCK_TSPIN_1 &&
		node.action.lock != LOCK_TSPIN_2 &&
		node.action.lock != LOCK_TSPIN_3 &&
		node.action.lock != LOCK_PC) {
		node.score.attack += this->weight.attack.waste_T;
	}
	if (quiescence &&
		node.state.hold == PIECE_T) {
		node.score.attack += this->weight.attack.waste_T;
	}

	// Waste I
	if (node.action.placement.type == PIECE_I &&
		node.action.lock != LOCK_CLEAR_4 &&
		node.action.lock != LOCK_PC) {
		node.score.attack += this->weight.attack.waste_I;
	}
	if (well_depth >= 4 &&
		node.state.hold == PIECE_I) {
		node.score.attack += this->weight.attack.waste_I;
	}
}

int Evaluator::well(BitBoard& board, int column_height[10], int& well_position)
{
	well_position = 0;
	for (int i = 1; i < 10; ++i) {
		if (column_height[i] < column_height[well_position]) {
			well_position = i;
		}
	}

	uint64_t mask = ~0b0;
	for (int i = 0; i < 10; ++i) {
		if (i == well_position) continue;
		mask = mask & board.column[i];
	}
	mask = mask >> column_height[well_position];
	return std::min(std::countr_one(mask), 8);
}

void Evaluator::bumpiness(int column_height[10], int well_index, int result[3])
{
	result[0] = 0;
	result[1] = 0;
	result[2] = 0;
	for (int i = 0; i < 9; ++i) {
		if (i == well_index - 1 || i == well_index) continue;
		result[0] += std::abs(column_height[i] - column_height[i + 1]);
		result[1] += (column_height[i] - column_height[i + 1]) * (column_height[i] - column_height[i + 1]);
		result[2] += (column_height[i] != column_height[i + 1]);
	}
	if (well_index != 0 && well_index != 9) {
		result[0] += std::abs(column_height[well_index - 1] - column_height[well_index + 1]);
		result[1] += (column_height[well_index - 1] - column_height[well_index + 1]) * (column_height[well_index - 1] - column_height[well_index + 1]);
		result[2] += (column_height[well_index - 1] != column_height[well_index + 1]);
	}
}

int Evaluator::hole(BitBoard& board, int column_height[10])
{
	int result = 0;
	for (int i = 0; i < 10; ++i) {
		result += column_height[i] - std::popcount(board.column[i]);
	}
	return result;
}

int Evaluator::blocked(BitBoard& board, int column_height[10])
{
	int result = 0;
	for (int i = 0; i < 10; ++i) {
		uint64_t hole_mask = (~board.column[i]) & (((uint64_t)1 << column_height[i]) - 1);
		while (hole_mask != 0)
		{
			int hole_mask_trz = std::countr_zero(hole_mask);
			result += std::min(column_height[i] - hole_mask_trz - 1, 6);
			hole_mask = hole_mask & (~((uint64_t)1 << hole_mask_trz));
		}
	}
	return result;
}


PieceData Evaluator::structure(BitBoard& board, int column_height[10])
{
	for (int _x = 0; _x < 8; ++_x) {
		if (column_height[_x + 0] > column_height[_x + 1] && column_height[_x + 0] + 1 < column_height[_x + 2]) {
			if (((board.column[_x + 0] >> (column_height[_x + 0] - 1)) & 0b111) == 0b001 &&
				((board.column[_x + 1] >> (column_height[_x + 0] - 1)) & 0b111) == 0b000 &&
				((board.column[_x + 2] >> (column_height[_x + 0] - 1)) & 0b111) == 0b101) {
				return {
					_x + 1,
					column_height[_x + 0],
					PIECE_T,
					PIECE_DOWN
				};
			}
		}
		if (column_height[_x + 2] > column_height[_x + 1] && column_height[_x + 2] + 1 < column_height[_x + 0]) {
			if (((board.column[_x + 0] >> (column_height[_x + 2] - 1)) & 0b111) == 0b101 &&
				((board.column[_x + 1] >> (column_height[_x + 2] - 1)) & 0b111) == 0b000 &&
				((board.column[_x + 2] >> (column_height[_x + 2] - 1)) & 0b111) == 0b001) {
				return {
					_x + 1,
					column_height[_x + 2],
					PIECE_T,
					PIECE_DOWN
				};
			}
		}
		if (column_height[_x + 1] >= column_height[_x + 0] && column_height[_x + 1] + 1 < column_height[_x + 2]) {
			if (((board.column[_x + 0] >> (column_height[_x + 1] - 3)) & 0b11000) == 0b00000 &&
				((board.column[_x + 1] >> (column_height[_x + 1] - 3)) & 0b11110) == 0b00100 &&
				((board.column[_x + 2] >> (column_height[_x + 1] - 3)) & 0b11111) == 0b10000 &&
				(
					board.is_occupied(_x + 1, column_height[_x + 1] - 3) ||
					(!board.is_occupied(_x + 1, column_height[_x + 1] - 3) && board.is_occupied(_x + 2, column_height[_x + 1] - 4))
					)) {
				return {
					_x + 2,
					column_height[_x + 1] - 2,
					PIECE_T,
					PIECE_LEFT
				};
			}
		}
		if (column_height[_x + 1] >= column_height[_x + 2] && column_height[_x + 1] + 1 < column_height[_x + 0]) {
			if (((board.column[_x + 0] >> (column_height[_x + 1] - 3)) & 0b11111) == 0b10000 &&
				((board.column[_x + 1] >> (column_height[_x + 1] - 3)) & 0b11110) == 0b00100 &&
				((board.column[_x + 2] >> (column_height[_x + 1] - 3)) & 0b11000) == 0b00000 &&
				(
					board.is_occupied(_x + 1, column_height[_x + 1] - 3) ||
					(!board.is_occupied(_x + 1, column_height[_x + 1] - 3) && board.is_occupied(_x + 0, column_height[_x + 1] - 4))
					)) {
				return {
					_x + 0,
					column_height[_x + 1] - 2,
					PIECE_T,
					PIECE_RIGHT
				};
			}
		}
	}
	return {
		-1,
		-1,
		PIECE_NONE,
		PIECE_UP
	};
}

bool Evaluator::quiescence(BitBoard& board, int column_height[10], int depth)
{
	bool result = false;
	for (int i = 0; i < depth; ++i) {
		BitBoard copy = board;
		PieceData quiet_piece = Evaluator::structure(copy, column_height);
		if (quiet_piece.type == PIECE_NONE) break;
		copy.place_piece(quiet_piece);
		int line_clear = copy.clear_line();
		if (line_clear >= 2) {
			board = copy;
			board.get_height(column_height);
			result = true;
		}
		else {
			break;
		}
	}
	return result;
}
