#include "eval.h"

Weight DEFAULT_WEIGHT()
{
    Weight result;

	result.attack.clear[0] = -199;
	result.attack.clear[1] = -139;
	result.attack.clear[2] = -93;
	result.attack.clear[3] = 303;
	result.attack.tspin[0] = 118;
	result.attack.tspin[1] = 419;
	result.attack.tspin[2] = 610;
	result.attack.perfect_clear = 5000;
	result.attack.waste_time = -162;
	result.attack.waste_T = -131;
	result.attack.waste_I = -110;
	result.attack.b2b = 96;
    result.attack.ren = 150;

	result.defence.max_height = -10;
	result.defence.max_height_top_half = -134;
	result.defence.max_height_top_quarter = -597;
	result.defence.bumpiness = -32;
	result.defence.bumpiness_s = -7;
	result.defence.bumpiness_t = 8;
	result.defence.row_transition = -5;
	result.defence.hole = -306;
	result.defence.hole_s = -5;
	result.defence.blocked = -14;
	result.defence.blocked_s = -1;
	result.defence.well = 46;
	result.defence.well_position[0] = 45;
	result.defence.well_position[1] = 5;
	result.defence.well_position[2] = 28;
	result.defence.well_position[3] = 57;
	result.defence.well_position[4] = 70;
	result.defence.well_position[5] = 42;
	result.defence.well_position[6] = 60;
	result.defence.well_position[7] = 10;
	result.defence.well_position[8] = -20;
	result.defence.well_position[9] = 51;
	result.defence.structure[0] = 107;
	result.defence.structure[1] = 394;
	result.defence.b2b = 36;

    return result;
}

void Evaluator::evaluate(Node* node, NodeState& node_state)
{
	/*
	* Defence evaluation
	*/
	node->score.defence = 0;
	BitBoard board = node_state.state.board;

	// Get column heights
	int column_height[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	board.get_height(column_height);

	// Height top of the board
	int max_height = *std::max_element(column_height, column_height + 10);
	node->score.defence += std::max(max_height - 10, 0) * this->weight.defence.max_height_top_half;
	node->score.defence += std::max(max_height - 15, 0) * this->weight.defence.max_height_top_quarter;

	// Quiescence
	int tspin_structure[2] = { 0, 0 };
	int quiescence_depth = (node_state.state.hold == PIECE_T);
	for (int i = 0; i < node_state.state.queue.get_size(); ++i) {
		if (node_state.state.queue[i] == PIECE_T) {
			++quiescence_depth;
			break;
		}
	}
	quiescence_depth += (node_state.state.queue.get_size() <= 3);
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
	node->score.defence += tspin_structure[0] * this->weight.defence.structure[0];
	node->score.defence += tspin_structure[1] * this->weight.defence.structure[1];

	// Height normal
	max_height = *std::max_element(column_height, column_height + 10);
	node->score.defence += max_height * this->weight.defence.max_height;

	// Well
	int well_position = 0;
	int well_depth = Evaluator::well(board, column_height, well_position);
	if (well_depth > 0) {
		node->score.defence += well_depth * this->weight.defence.well;
		node->score.defence += this->weight.defence.well_position[well_position];
	}
	else {
		well_position = -1;
	}

	// Bumpiness
	int bumpiness[3];
	Evaluator::bumpiness(column_height, well_position, well_depth, bumpiness);
	node->score.defence += bumpiness[0] * this->weight.defence.bumpiness;
	node->score.defence += bumpiness[1] * this->weight.defence.bumpiness_s;
	node->score.defence += bumpiness[2] * this->weight.defence.bumpiness_t;

	// Row transition
	int row_transition = Evaluator::row_transition(board, column_height);
	node->score.defence += row_transition * this->weight.defence.row_transition;

	// Hole
	int hole = Evaluator::hole(board, column_height);
	node->score.defence += hole * this->weight.defence.hole;
	node->score.defence += hole * hole * this->weight.defence.hole_s;

	// Blocked hole
	int blocked[2];
	Evaluator::blocked(board, column_height, blocked);
	node->score.defence += blocked[0] * this->weight.defence.blocked;
	node->score.defence += blocked[1] * this->weight.defence.blocked_s;

	// B2B
    if (node_state.state.b2b > 0) {
	    node->score.defence += this->weight.defence.b2b;
    }


	/*
	* Attack evaluation
	*/
    node->score.attack = 0;

	// Attacks
	switch (node_state.lock.type)
	{
	case LOCK_CLEAR_1:
		node->score.attack += this->weight.attack.clear[0];
		break;
	case LOCK_CLEAR_2:
		node->score.attack += this->weight.attack.clear[1];
		break;
	case LOCK_CLEAR_3:
		node->score.attack += this->weight.attack.clear[2];
		break;
	case LOCK_CLEAR_4:
		node->score.attack += this->weight.attack.clear[3];
		break;
	case LOCK_TSPIN_1:
		node->score.attack += this->weight.attack.tspin[0];
		break;
	case LOCK_TSPIN_2:
		node->score.attack += this->weight.attack.tspin[1];
		break;
	case LOCK_TSPIN_3:
		node->score.attack += this->weight.attack.tspin[2];
		break;
	case LOCK_PC:
		node->score.attack += this->weight.attack.perfect_clear;
		break;;
	default:
		break;
	}

	// Waste time
	if (node_state.lock.softdrop &&
		node_state.lock.type != LOCK_TSPIN_1 &&
		node_state.lock.type != LOCK_TSPIN_2 &&
		node_state.lock.type != LOCK_TSPIN_3 &&
		node_state.lock.type != LOCK_PC)
		node->score.attack += this->weight.attack.waste_time;
	if (node_state.lock.type == LOCK_CLEAR_1 ||
		node_state.lock.type == LOCK_CLEAR_2 ||
		node_state.lock.type == LOCK_CLEAR_3)
		node->score.attack += this->weight.attack.waste_time;

	// Waste T
	if (node->placement.type == PIECE_T &&
		node_state.lock.type != LOCK_TSPIN_1 &&
		node_state.lock.type != LOCK_TSPIN_2 &&
		node_state.lock.type != LOCK_TSPIN_3 &&
		node_state.lock.type != LOCK_PC) {
		node->score.attack += this->weight.attack.waste_T;
	}

	// Waste I
	if (node->placement.type == PIECE_I &&
		node_state.lock.type != LOCK_CLEAR_4 &&
		node_state.lock.type != LOCK_PC) {
		node->score.attack += this->weight.attack.waste_I;
	}

	// B2B
    if (node_state.state.b2b > 0) {
	    node->score.attack += this->weight.attack.b2b;
    }

    // REN
    node->score.attack += REN_LUT[std::min(node_state.state.ren, MAX_COMBO_TABLE_SIZE - 1)] * this->weight.attack.ren;
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
					(int8_t)(_x + 1),
					(int8_t)column_height[_x + 0],
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
					(int8_t)(_x + 1),
					(int8_t)column_height[_x + 2],
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
					(int8_t)(_x + 2),
					(int8_t)(column_height[_x + 1] - 2),
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
					(int8_t)_x,
					(int8_t)(column_height[_x + 1] - 2),
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

int Evaluator::spike(NodeState& root, Node& node, NodeState& node_state)
{
	// Get line clear
	BitBoard root_copy = root.state.board;
	root_copy.place_piece(node.placement);
	int line_clear = root_copy.clear_line();

	// Get attack
	int attack = 0;
	switch (node_state.lock.type)
	{
	case LOCK_CLEAR_2:
		attack = 1;
		break;
	case LOCK_CLEAR_3:
		attack = 2;
		break;
	case LOCK_CLEAR_4:
		attack = 4;
		break;
	case LOCK_TSPIN_1:
		attack = 2;
		break;
	case LOCK_TSPIN_2:
		attack = 4;
		break;
	case LOCK_TSPIN_3:
		attack = 6;
		break;
	case LOCK_PC:
		attack = 10;
		break;
	default:
		break;
	}
	if (root.state.b2b > 0 &&
		(
			node_state.lock.type == LOCK_CLEAR_4 ||
			node_state.lock.type == LOCK_TSPIN_1 ||
			node_state.lock.type == LOCK_TSPIN_2 ||
			node_state.lock.type == LOCK_TSPIN_3
			)
		) {
		++attack;
	}
	if (line_clear > 0) {
		attack += REN_LUT[std::min(root.state.ren, MAX_COMBO_TABLE_SIZE - 1)];
	}

	return line_clear + attack;
}