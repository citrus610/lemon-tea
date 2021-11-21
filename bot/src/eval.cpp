#include "eval.h"

void Weight::standard()
{
	attack.clear[0] = -150;
	attack.clear[1] = -100;
	attack.clear[2] = -50;
	attack.clear[3] = 400;
	attack.tspin[0] = 100;
	attack.tspin[1] = 400;
	attack.tspin[2] = 600;
	attack.perfect_clear = 1000;
	attack.waste_time = -150;
	attack.waste_T = -150;
	attack.waste_I = 0;

	defence.max_height = -40;
	defence.max_height_top_half = -150;
	defence.max_height_top_quarter = -600;
	defence.bumpiness = -25;
	defence.bumpiness_s = -7;
	defence.bumpiness_t = -10;
	defence.row_transition = -5;
	defence.hole = -150;
	defence.hole_s = -3;
	defence.blocked = -17;
	defence.blocked_s = -1;
	defence.well = 57;
	defence.well_position[0] = 20;
	defence.well_position[1] = 23;
	defence.well_position[2] = 23;
	defence.well_position[3] = 50;
	defence.well_position[4] = 59;
	defence.well_position[5] = 21;
	defence.well_position[6] = 59;
	defence.well_position[7] = 10;
	defence.well_position[8] = -10;
	defence.well_position[9] = 24;
	defence.structure[0] = 10;
	defence.structure[1] = 400;
	defence.b2b = 100;
	defence.ren = 150;
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

	// Height top of the board
	int max_height = *std::max_element(column_height, column_height + 10);
	node.score.defence += std::max(max_height - 10, 0) * this->weight.defence.max_height_top_half;
	node.score.defence += std::max(max_height - 15, 0) * this->weight.defence.max_height_top_quarter;

	// Quiescence
	int tspin_structure[2] = { 0, 0 };
	int quiescence_depth = (node.state.current == PIECE_T) + (node.state.hold == PIECE_T);
	for (int i = node.state.next; i < queue_count; ++i) {
		quiescence_depth += (queue[i] == PIECE_T);
	}
	bool quiescence = Evaluator::quiescence(board, column_height, quiescence_depth, tspin_structure);

	// Structure
	if (quiescence_depth < 1) {
		PieceData relevent_structure = Evaluator::structure(board, column_height);
		if (relevent_structure.type == PIECE_T) {
			if (relevent_structure.rotation == PIECE_DOWN)
				++tspin_structure[0];
			else
				++tspin_structure[1];
		}
	}
	node.score.defence += tspin_structure[0] * this->weight.defence.structure[0];
	node.score.defence += tspin_structure[1] * this->weight.defence.structure[1];

	// Height normal
	max_height = *std::max_element(column_height, column_height + 10);
	node.score.defence += max_height * this->weight.defence.max_height;

	// Well
	int well_position = 0;
	int well_depth = Evaluator::well(board, column_height, well_position);
	if (well_depth > 0) {
		node.score.defence += well_depth * this->weight.defence.well;
		node.score.defence += this->weight.defence.well_position[well_position];
	}
	else {
		well_position = -1;
	}

	// Bumpiness
	int bumpiness[3];
	Evaluator::bumpiness(column_height, well_position, well_depth, bumpiness);
	node.score.defence += bumpiness[0] * this->weight.defence.bumpiness;
	node.score.defence += bumpiness[1] * this->weight.defence.bumpiness_s;
	node.score.defence += bumpiness[2] * this->weight.defence.bumpiness_t;

	// Row transition
	int row_transition = Evaluator::row_transition(board, column_height);
	node.score.defence += row_transition * this->weight.defence.row_transition;

	// Hole
	int hole = Evaluator::hole(board, column_height);
	node.score.defence += hole * this->weight.defence.hole;
	node.score.defence += hole * hole * this->weight.defence.hole_s;

	// Blocked hole
	int blocked[2];
	Evaluator::blocked(board, column_height, blocked);
	node.score.defence += blocked[0] * this->weight.defence.blocked;
	node.score.defence += blocked[1] * this->weight.defence.blocked_s;

	// B2B
	node.score.defence += node.state.b2b * this->weight.defence.b2b;

	// Ren
	node.score.defence += node.state.ren * node.state.ren / 5 * this->weight.defence.ren;


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

	// Waste I
	if (node.action.placement.type == PIECE_I &&
		node.action.lock != LOCK_CLEAR_4 &&
		node.action.lock != LOCK_PC) {
		node.score.attack += this->weight.attack.waste_I;
	}
}

void Evaluator::evaluate_forecast(NodeForecast& node_forecast)
{
	node_forecast.score = 0;
	for (int i = 0; i < node_forecast.bag_count; ++i) {
		node_forecast.score += node_forecast.children[i].score.attack;
		node_forecast.score += node_forecast.children[i].score.defence;
	}
	node_forecast.score = node_forecast.score / node_forecast.bag_count;
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
	return std::min(std::countr_one(mask), 15);
}

void Evaluator::bumpiness(int column_height[10], int well_position, int well_depth, int result[3])
{
	result[0] = 0;
	result[1] = 0;
	result[2] = 0;
	int pre_index = 0;
	if (well_position == 0) pre_index = 1;
	for (int i = 1; i < 10; ++i) {
		if (i == well_position) continue;
		int height_different = std::abs(column_height[pre_index] - column_height[i]);
		result[0] += height_different;
		result[1] += height_different * height_different;
		result[2] += height_different > 0;
		pre_index = i;
	}
}

int Evaluator::row_transition(BitBoard& board, int column_height[10])
{
	int result = 0;
	for (int i = 0; i < 9; ++i) {
		uint64_t xor_column = board.column[i] ^ board.column[i + 1];
		result += std::popcount(xor_column);
	}
	result += 64 - std::popcount(board.column[0]);
	result += 64 - std::popcount(board.column[9]);
	return result;
}

int Evaluator::hole(BitBoard& board, int column_height[10])
{
	int result = 0;
	for (int i = 0; i < 10; ++i) {
		result += column_height[i] - std::popcount(board.column[i]);
	}
	return result;
}

void Evaluator::blocked(BitBoard& board, int column_height[10], int blocked[2])
{
	blocked[0] = 0;
	blocked[1] = 0;
	for (int i = 0; i < 10; ++i) {
		uint64_t hole_mask = (~board.column[i]) & (((uint64_t)1 << column_height[i]) - 1);
		while (hole_mask != 0)
		{
			int hole_mask_trz = std::countr_zero(hole_mask);
			int blocked_count = std::min(column_height[i] - hole_mask_trz - 1, 6);
			blocked[0] += blocked_count;
			blocked[1] += blocked_count * blocked_count;
			hole_mask = hole_mask & (~((uint64_t)1 << hole_mask_trz));
		}
	}
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

bool Evaluator::quiescence(BitBoard& board, int column_height[10], int depth, int tspin_structure[2])
{
	bool result = false;
	for (int i = 0; i < depth; ++i) {
		BitBoard copy = board;
		PieceData quiet_piece = Evaluator::structure(copy, column_height);
		if (quiet_piece.type == PIECE_NONE) break;
		if (quiet_piece.rotation == PIECE_DOWN)
			++tspin_structure[0];
		else
			++tspin_structure[1];
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
