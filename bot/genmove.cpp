#include "genmove.h"

/*
* Generate possible (not all) piece positions for the current board and piece state
* How this works:
*  - STEP 1: Generate all hard drop positions
*  - STEP 2: For all hard drop positions, try move left right or rotate piece to find new soft drops positions
*  - STEP 3: For all new soft drop positions, again try move left right or rotate piece to find newer positions
*  - STEP 4: Repeat STEP 3 for N time, usually N = 2 to save time;
*/
void genmove::generate(bitboard& board, piece_type piece, piece_data result[MAX_MOVE_GEN_COUNT], int& result_count)
{
	result_count = 0;
	piece_data soft_drop_list[MAX_SOFT_DROP_DEPTH][MAX_MOVE_GEN_COUNT];
	int soft_drop_count[MAX_SOFT_DROP_DEPTH];


	/* STEP 1: Find all hard drop positions */

	// Count maximum rotation count
	int max_rotation_count = 4;

	if (piece == PIECE_O)
		max_rotation_count = 1;
	else if (piece == PIECE_I || piece == PIECE_S || piece == PIECE_Z)
		max_rotation_count = 2;

	// Find possible initial y piece positions
	int y_init_pos = 19;
	if (board.is_occupied(4, y_init_pos)) { // If piece collides with the board at the starting positions, then return null
		++y_init_pos;
		if (board.is_occupied(4, y_init_pos)) return;
	}

	// Drop pieces and find hard drop positions
	for (int rotation_state = 0; rotation_state < max_rotation_count; ++rotation_state) { // For every rotation state
		for (int x_pos = 4; x_pos >= 0; --x_pos) { // For every x position move left
			// If piece is colliding with board then move on
			if (board.is_colliding(x_pos, y_init_pos, piece, (piece_rotation)rotation_state)) break;

			// Create a simulating piece
			piece_data simulating_piece = { x_pos, y_init_pos, piece, (piece_rotation)rotation_state, false };

			// Calculate drop distance, if negative: move on, else: drop piece
			int drop_distance = board.get_drop_distance(simulating_piece);
			if (drop_distance < 0) continue;
			simulating_piece.y -= drop_distance;

			// Push simulating piece to piece list
			result[result_count] = simulating_piece;
			++result_count;
		}
		for (int x_pos = 5; x_pos < 10; ++x_pos) { // For every x position move right
			// If piece is colliding with board then move on
			if (board.is_colliding(x_pos, y_init_pos, piece, (piece_rotation)rotation_state)) break;

			// Create a simulating piece
			piece_data simulating_piece = { x_pos, y_init_pos, piece, (piece_rotation)rotation_state, false };

			// Calculate drop distance, if negative: move on, else: drop piece
			int drop_distance = board.get_drop_distance(simulating_piece);
			if (drop_distance < 0) continue;
			simulating_piece.y -= drop_distance;

			// Push simulating piece to piece list
			result[result_count] = simulating_piece;
			++result_count;
		}
	}

	/* STEP 2: For all hard drop positions, try move left right or rotate piece to find new soft drops positions, repeat this step N times, default N = 2 */
	if (result_count == 0) return;
	generate_from_list(board, piece, result, result_count, soft_drop_list[0], soft_drop_count[0]);
	for (int i = 0; i < MAX_SOFT_DROP_DEPTH - 1; ++i) {
		generate_from_list(board, piece, soft_drop_list[i], soft_drop_count[i], soft_drop_list[i + 1], soft_drop_count[i + 1]);
	}


	/* STEP 3: Push all new positions to the result queue */
	for (int i = 0; i < MAX_SOFT_DROP_DEPTH; ++i) {
		memcpy(result + result_count, soft_drop_list[i], soft_drop_count[i] * sizeof(piece_data));
		result_count += soft_drop_count[i];
	}
}

/*
* Generate new positions from a list of previous positions
*/
void genmove::generate_from_list(bitboard& board, piece_type piece, piece_data pre[MAX_MOVE_GEN_COUNT], int& pre_count, piece_data next[MAX_MOVE_GEN_COUNT], int& next_count)
{
	next_count = 0;

	// Number of previous positions shouldn't be 0
	if (pre_count == 0) return;

	// Find new positions from previous positions
	for (int i = 0; i < pre_count; ++i) {
		piece_data simulating_piece_try_right = pre[i];
		piece_data simulating_piece_try_left = pre[i];
		piece_data simulating_piece_try_cw = pre[i];
		piece_data simulating_piece_try_ccw = pre[i];

		// Try move piece right
		if (board.piece_try_right(simulating_piece_try_right)) {
			if (!board.is_above_stack(simulating_piece_try_right)) {
				board.piece_try_down(simulating_piece_try_right);
				simulating_piece_try_right.soft_drop = true;
				if (is_piece_in_list(simulating_piece_try_right, next, next_count) == -1) {
					next[next_count] = simulating_piece_try_right;
					++next_count;
				}
			}
		}

		// Try move piece left
		if (board.piece_try_left(simulating_piece_try_left)) {
			if (!board.is_above_stack(simulating_piece_try_left)) {
				board.piece_try_down(simulating_piece_try_left);
				simulating_piece_try_left.soft_drop = true;
				if (is_piece_in_list(simulating_piece_try_left, next, next_count) == -1) {
					next[next_count] = simulating_piece_try_left;
					++next_count;
				}
			}
		}

		// Try rotate piece
		if (piece != PIECE_O) {
			// Try rotate piece cw
			if (board.piece_try_rotate(simulating_piece_try_cw, true)) {
				if (!board.is_above_stack(simulating_piece_try_cw)) {
					board.piece_try_down(simulating_piece_try_cw);
					simulating_piece_try_cw.soft_drop = true;
					if (is_piece_in_list(simulating_piece_try_cw, next, next_count) == -1) {
						next[next_count] = simulating_piece_try_cw;
						++next_count;
					}
				}
			}

			// Try rotate piece ccw
			if (board.piece_try_rotate(simulating_piece_try_ccw, false)) {
				if (!board.is_above_stack(simulating_piece_try_ccw)) {
					board.piece_try_down(simulating_piece_try_ccw);
					simulating_piece_try_ccw.soft_drop = true;
					if (is_piece_in_list(simulating_piece_try_ccw, next, next_count) == -1) {
						next[next_count] = simulating_piece_try_ccw;
						++next_count;
					}
				}
			}
		}

		// If piece is I, S, Z, we have to mirror them to avoid missing new positions
		if (piece == PIECE_I || piece == PIECE_S || piece == PIECE_Z) {
			piece_data simulating_mirror_piece_try_cw = pre[i];
			simulating_mirror_piece_try_cw.mirror();
			piece_data simulating_mirror_piece_try_ccw = simulating_mirror_piece_try_cw;

			// Try rotate piece cw
			if (board.piece_try_rotate(simulating_mirror_piece_try_cw, true)) {
				if (!board.is_above_stack(simulating_mirror_piece_try_cw)) {
					board.piece_try_down(simulating_mirror_piece_try_cw);
					simulating_mirror_piece_try_cw.soft_drop = true;
					if (is_piece_in_list(simulating_mirror_piece_try_cw, next, next_count) == -1) {
						next[next_count] = simulating_mirror_piece_try_cw;
						++next_count;
					}
				}
			}

			// Try rotate piece ccw
			if (board.piece_try_rotate(simulating_mirror_piece_try_ccw, false)) {
				if (!board.is_above_stack(simulating_mirror_piece_try_ccw)) {
					board.piece_try_down(simulating_mirror_piece_try_ccw);
					simulating_mirror_piece_try_ccw.soft_drop = true;
					if (is_piece_in_list(simulating_mirror_piece_try_ccw, next, next_count) == -1) {
						next[next_count] = simulating_mirror_piece_try_ccw;
						++next_count;
					}
				}
			}
		}
	}
}

/*
* Return index if current piece is already in list
* Return -1 if false
*/
int genmove::is_piece_in_list(piece_data& piece, piece_data list[MAX_MOVE_GEN_COUNT], int& list_count)
{
	for (int i = 0; i < list_count; ++i) {
		piece_data normalized_piece = piece;
		normalized_piece.normalize();
		piece_data normalized_piece_in_list = list[i];
		normalized_piece_in_list.normalize();
		if (normalized_piece == normalized_piece_in_list) return i;
	}
	return -1;
}
