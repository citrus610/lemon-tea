#pragma once

/*
* Contain board & piece representation
*/

#include "def.h"

/*
* Piece's data
*/
struct PieceData
{
	int8_t x = 0;
	int8_t y = 0;
	PieceType type = PIECE_NONE;
	PieceRotation rotation = PIECE_UP;

	void normalize();
	void mirror();
	bool operator == (PieceData& other) {
		return (type == other.type) && (x == other.x) && (y == other.y) && (rotation == other.rotation);
	};
};

/*
* Board's representation
* An array of 10 uint64_t for 10 columns
*/
struct BitBoard
{
	uint64_t column[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	void get_height(int height[10]);
	int get_drop_distance(PieceData& piece);
	uint64_t get_mask();
	int clear_line();
	bool is_occupied(const int8_t& x, const int8_t& y);
	bool is_colliding(const int8_t& x, const int8_t& y, const PieceType& type, const PieceRotation& rotation);
	bool is_above_stack(PieceData& piece);
	bool is_t_spin(PieceData& piece);
	void place_piece(PieceData& piece);
	bool piece_try_right(PieceData& piece);
	bool piece_try_left(PieceData& piece);
	bool piece_try_rotate(PieceData& piece, bool is_cw);
	bool piece_try_down(PieceData& piece);
	bool operator == (const BitBoard& other) {
		for (int i = 0; i < 10; ++i) {
			if (column[i] != other.column[i]) return false;
		}
		return true;
	}
};