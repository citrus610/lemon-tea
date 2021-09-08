#ifndef BITBOARD_H
#define BITBOARD_H

#include "def.h"

struct piece_data
{
	int x = 0, y = 0;
	piece_type type = PIECE_NONE;
	piece_rotation rotation = PIECE_UP;
	bool soft_drop = false;

	void normalize();
	void mirror();
	bool operator == (piece_data& other) {
		return (type == other.type) && (x == other.x) && (y == other.y) && (rotation == other.rotation);
	};
};

/*
NOTE TO SELF: The coordinate (x = 0, y = 0) points to the bottom left cell of the board
*/
struct bitboard
{
	uint64_t column[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	void get_height(int height[10]);
	int get_drop_distance(piece_data& piece);
	uint64_t get_mask();
	int clear_line();
	bool is_occupied(const int& x, const int& y);
	bool is_colliding(const int& x, const int& y, const piece_type& type, const piece_rotation& rotation);
	bool is_above_stack(piece_data& piece);
	bool is_t_spin(piece_data& piece);
	void place_piece(piece_data& piece);
	bool piece_try_right(piece_data& piece);
	bool piece_try_left(piece_data& piece);
	bool piece_try_rotate(piece_data& piece, bool is_cw);
	bool piece_try_down(piece_data& piece);

	bool operator == (const bitboard& other) {
		for (int i = 0; i < 10; ++i) {
			if (column[i] != other.column[i]) return false;
		}
		return true;
	};
};

#endif // BITBOARD_H