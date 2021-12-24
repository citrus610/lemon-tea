#include "movegen.h"

void MoveGenerator::generate(BitBoard& board, PieceType piece, PieceData result[MAX_MOVE_GENERATOR], int& result_count)
{
	result_count = 0;

	// Check fast mode
	bool fast_drop = true;
	for (int i = 0; i < 10; ++i) {
		if (64 - std::countl_zero(board.column[i]) > 16) {
			fast_drop = false;
			break;
		}
	}

	// Find all hardrop positions
	PieceData harddrop[MAX_MOVE_GENERATOR];
	int harddrop_count = 0;

	if (fast_drop) {
		int8_t rotation_count = 4;
		if (piece == PIECE_O) rotation_count = 1;

		int8_t piece_max_rotation = 4;
		if (piece == PIECE_I || piece == PIECE_Z || piece == PIECE_S) piece_max_rotation = 2;
		if (piece == PIECE_O) piece_max_rotation = 1;

		for (int8_t rotation = 0; rotation < rotation_count; ++rotation) {
			for (int8_t x = 0; x < 10; ++x) {
				if (board.is_colliding(x, (int8_t)19, piece, (PieceRotation)rotation)) {
					continue;
				}

				PieceData new_location = {
					x,
					(int8_t)19,
					piece,
					(PieceRotation)rotation
				};

				new_location.y -= (int8_t)board.get_drop_distance(new_location);

				harddrop[harddrop_count] = new_location;
				++harddrop_count;

				if (rotation < piece_max_rotation) {
					result[result_count] = new_location;
					++result_count;
				}
			}
		}
	}
	else {
		PieceData open[MAX_MOVE_GENERATOR * 2];
		int open_count = 0;
		PieceData close[MAX_MOVE_GENERATOR * 2];
		int close_count = 0;

		PieceData init_location = {
			(int8_t)4,
			(int8_t)19,
			piece,
			PIECE_UP
		};

		if (board.is_colliding((int8_t)4, (int8_t)19, piece, PIECE_UP)) {
			init_location.y = (int8_t)20;
			if (board.is_colliding((int8_t)4, (int8_t)20, piece, PIECE_UP)) {
				return;
			}
		}

		open[0] = init_location;
		++open_count;

		while (open_count > 0)
		{
			PieceData parent = open[0];

			open[0] = open[open_count - 1];
			--open_count;

			PieceData children[4];
			int children_count = 0;

			MoveGenerator::expand(board, parent, children, children_count);

			for (int i = 0; i < children_count; ++i) {
				if (children[i].y < (int8_t)22 &&
					MoveGenerator::is_in(children[i], open, open_count, true) == -1 &&
					MoveGenerator::is_in(children[i], close, close_count, true) == -1) {
					open[open_count] = children[i];
					++open_count;

					children[i].y -= (int8_t)board.get_drop_distance(children[i]);
					if (children[i].y >= (int8_t)20) continue;
					if (MoveGenerator::is_in(children[i], harddrop, harddrop_count, false) == -1) {
						harddrop[harddrop_count] = children[i];
						++harddrop_count;
					}
				}

			}

			close[close_count] = parent;
			++close_count;
		}

		memcpy(result, harddrop, harddrop_count * sizeof(PieceData));
		result_count = harddrop_count;
	}

	// Find some soft drop positions
	PieceData softdrop[2][MAX_MOVE_GENERATOR];
	int softdrop_count[2] = { 0, 0 };

	// Level 1
	for (int i = 0; i < harddrop_count; ++i) {
		PieceData children[4];
		int children_count = 0;

		MoveGenerator::expand(board, harddrop[i], children, children_count);

		for (int k = 0; k < children_count; ++k) {
			if (board.is_above_stack(children[k])) continue;
			children[k].y -= (int8_t)board.get_drop_distance(children[k]);
			if (MoveGenerator::is_in(children[k], softdrop[0], softdrop_count[0], false) == -1) {
				softdrop[0][softdrop_count[0]] = children[k];
				++softdrop_count[0];
			}
		}
	}
	memcpy(result + result_count, softdrop[0], softdrop_count[0] * sizeof(PieceData));
	result_count += softdrop_count[0];

	// Level 2
	for (int i = 0; i < softdrop_count[0]; ++i) {
		PieceData children[4];
		int children_count = 0;

		MoveGenerator::expand(board, softdrop[0][i], children, children_count);

		for (int k = 0; k < children_count; ++k) {
			if (board.is_above_stack(children[k])) continue;
			children[k].y -= (int8_t)board.get_drop_distance(children[k]);
			if (MoveGenerator::is_in(children[k], softdrop[0], softdrop_count[0], false) == -1 &&
				MoveGenerator::is_in(children[k], softdrop[1], softdrop_count[1], false) == -1) {
				softdrop[1][softdrop_count[1]] = children[k];
				++softdrop_count[1];
			}
		}
	}
	memcpy(result + result_count, softdrop[1], softdrop_count[1] * sizeof(PieceData));
	result_count += softdrop_count[1];
}

int MoveGenerator::is_in(PieceData& placement, PieceData list[MAX_MOVE_GENERATOR], int& list_count, bool exact)
{
	for (int i = 0; i < list_count; ++i) {
		if (exact) {
			if (placement == list[i]) return i;
		}
		else {
			PieceData normalized_piece = placement;
			normalized_piece.normalize();
			PieceData normalized_piece_in_list = list[i];
			normalized_piece_in_list.normalize();
			if (normalized_piece == normalized_piece_in_list) return i;
		}
	}
	return -1;
}

void MoveGenerator::expand(BitBoard& board, PieceData& piece, PieceData result[4], int& result_count)
{
	result_count = 0;

	PieceData right = piece;
	PieceData left = piece;

	if (board.piece_try_right(right)) {
		result[result_count] = right;
		++result_count;
	}

	if (board.piece_try_left(left)) {
		result[result_count] = left;
		++result_count;
	}

	if (piece.type != PIECE_O) {
		PieceData cw = piece;
		PieceData ccw = piece;

		if (board.piece_try_rotate(cw, true)) {
			result[result_count] = cw;
			++result_count;
		}

		if (board.piece_try_rotate(ccw, false)) {
			result[result_count] = ccw;
			++result_count;
		}
	}
}
