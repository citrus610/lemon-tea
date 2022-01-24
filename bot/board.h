#pragma once

#include "def.h"

namespace LemonTea
{

class Board;

class Piece
{
public:
    int8_t x = 0;
    int8_t y = 0;
    PieceType type = PIECE_NONE;
    PieceRotation rotation = PIECE_UP;
public:
    bool operator == (Piece& other);
public:
    bool move_right(Board& board);
    bool move_left(Board& board);
    bool move_cw(Board& board);
    bool move_ccw(Board& board);
    bool move_down(Board& board);
    void move_drop(Board& board);
public:
    void place(Board& board);
public:
    void normalize();
    void mirror();
public:
    Piece get_normalize();
    Piece get_mirror();
};

class Board
{
public:
    uint64_t data[10] = { 0 };
public:
    uint64_t& operator [] (int index);
    bool operator == (Board& other);
public:
    void get_height(int height[10]);
    int get_drop_distance(Piece& piece);
    uint64_t get_mask();
public:
    bool is_occupied(const int8_t& x, const int8_t& y);
	bool is_colliding(const int8_t& x, const int8_t& y, const PieceType& type, const PieceRotation& rotation);
	bool is_colliding(Piece& piece);
	bool is_above_stack(Piece& piece);
	bool is_tspin(Piece& piece);
    bool is_perfect();
public:
    int clear_line();
};

};