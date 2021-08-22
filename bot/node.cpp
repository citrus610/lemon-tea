#include "node.h"

void node::attemp(node& parent, piece_data& piece, bool is_hold, piece_type queue[16], int& queue_count)
{
	*this = parent;
	pre_structure[0] = parent.structure[0];
	pre_structure[1] = parent.structure[1];

	// Update hold piece
	if (is_hold) {
		if (hold == PIECE_NONE)
			++next;
		hold = current;
	}

	// Update current piece
	if (next < queue_count) {
		current = queue[next];
		++next;
	}
	else {
		current = PIECE_NONE;
	}

	// Check t spin, place piece and clear line
	bool is_t_spin = board.is_t_spin(piece);
	board.place_piece(piece);
	int line_cleared = board.clear_line();

	// Update ren, b2b and lock data
	if (line_cleared > 0) {
		++ren;
		// T spin
		if (is_t_spin) {
			++b2b;
			lock = (lock_data)((int)LOCK_TSPIN_1 + (line_cleared - 1));
			++tspin[line_cleared - 1];
		}
		else {
			// Perfect clear
			if (board.column[0] == 0 &&
				board.column[1] == 0 &&
				board.column[2] == 0 &&
				board.column[3] == 0 &&
				board.column[4] == 0 &&
				board.column[5] == 0 &&
				board.column[6] == 0 &&
				board.column[7] == 0 &&
				board.column[8] == 0 &&
				board.column[9] == 0) {
				lock = LOCK_PC;
				++pc;
			}
			else {
				// Normal clear line
				if (line_cleared == 4)
					++b2b;
				else
					b2b = 0;
				lock = (lock_data)((int)LOCK_CLEAR_1 + (line_cleared - 1));
				++clear[line_cleared - 1];
			}
		}
	}
	else {
		ren = 0;
		lock = LOCK_NONE;
	}

	max_b2b = std::max(max_b2b, b2b);
	max_ren = std::max(max_ren, ren);

	if (piece.type == PIECE_T && lock != LOCK_TSPIN_1 && lock != LOCK_TSPIN_2 && lock != LOCK_TSPIN_3 && lock != LOCK_PC)
		++waste_T;
	if (piece.type == PIECE_I && lock != LOCK_CLEAR_4 && lock != LOCK_PC && lock != LOCK_NONE)
		++waste_I;
}
