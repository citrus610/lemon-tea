#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <chrono>
#include <vector>
#include <array>
#include <cassert>
#include <algorithm>
#include <bitset>
#include <bit>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "vec.h"
#include "arrayvec.h"

enum PieceType : int8_t
{
	PIECE_I,
	PIECE_J,
	PIECE_L,
	PIECE_O,
	PIECE_S,
	PIECE_T,
	PIECE_Z,
	PIECE_NONE
};

enum PieceRotation : int8_t
{
	PIECE_UP,
	PIECE_RIGHT,
	PIECE_DOWN,
	PIECE_LEFT
};

enum LockType : int8_t
{
	LOCK_NONE,
	LOCK_CLEAR_1,
	LOCK_CLEAR_2,
	LOCK_CLEAR_3,
	LOCK_CLEAR_4,
	LOCK_TSPIN_1,
	LOCK_TSPIN_2,
	LOCK_TSPIN_3,
	LOCK_PC
};

enum MoveType : int8_t
{
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_CW,
	MOVE_CCW,
	MOVE_DOWN
};

/*
* NOTE TO SELF: piece_lut [ PIECE_INDEX ] [ PIECE_ROTATION_STATE ] [ CELL_INDEX ] [ CELL_X / CELL_Y ];
*/
constexpr int8_t PIECE_LUT[7][4][4][2] =
{
	// PIECE I
	{
		{{-1, 0}, {0, 0}, {1, 0}, {2, 0}},      // UP
		{{0, 1}, {0, 0}, {0, -1}, {0, -2}},     // RIGHT
		{{1, 0}, {0, 0}, {-1, 0}, {-2, 0}},     // DOWN
		{{0, -1}, {0, 0}, {0, 1}, {0, 2}}       // LEFT
	},

	// PIECE J
	{
		{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}},     // UP
		{{0, 1}, {0, 0}, {0, -1}, {1, 1}},      // RIGHT
		{{1, 0}, {0, 0}, {-1, 0}, {1, -1}},     // DOWN
		{{0, -1}, {0, 0}, {0, 1}, {-1, -1}}     // LEFT
	},

	// PIECE L
	{
		{{-1, 0}, {0, 0}, {1, 0}, {1, 1}},      // UP
		{{0, 1}, {0, 0}, {0, -1}, {1, -1}},     // RIGHT
		{{1, 0}, {0, 0}, {-1, 0}, {-1, -1}},    // DOWN
		{{0, -1}, {0, 0}, {0, 1}, {-1, 1}}      // LEFT
	},

	// PIECE O
	{
		{{0, 0}, {1, 0}, {0, 1}, {1, 1}},       // UP
		{{0, 0}, {0, -1}, {1, 0}, {1, -1}},     // RIGHT
		{{0, 0}, {-1, 0}, {0, -1}, {-1, -1}},   // DOWN
		{{0, 0}, {0, 1}, {-1, 0}, {-1, 1}}      // LEFT
	},

	// PIECE S
	{
		{{-1, 0}, {0, 0}, {0, 1}, {1, 1}},      // UP
		{{0, 1}, {0, 0}, {1, 0}, {1, -1}},      // RIGHT
		{{1, 0}, {0, 0}, {0, -1}, {-1, -1}},    // DOWN
		{{0, -1}, {0, 0}, {-1, 0}, {-1, 1}}     // LEFT
	},

	// PIECE T
	{
		{{-1, 0}, {0, 0}, {1, 0}, {0, 1}},      // UP
		{{0, 1}, {0, 0}, {0, -1}, {1, 0}},      // RIGHT
		{{1, 0}, {0, 0}, {-1, 0}, {0, -1}},     // DOWN
		{{0, -1}, {0, 0}, {0, 1}, {-1, 0}}      // LEFT
	},

	// PIECE Z
	{
		{{-1, 1}, {0, 1}, {0, 0}, {1, 0}},      // UP
		{{1, 1}, {1, 0}, {0, 0}, {0, -1}},      // RIGHT
		{{1, -1}, {0, -1}, {0, 0}, {-1, 0}},    // DOWN
		{{-1, -1}, {-1, 0}, {0, 0}, {0, 1}}     // LEFT
	}
};

/*
* NOTE TO SELF
*  - only 2 tables for I piece and the others, O piece can't rotate
*  - srs_lut [ 0 - PIECE_I, 1 - OTHERS ] [ PIECE_ROTATION_STATE ] [ OFFSET_INDEX ] [ CELL_X / CELL_Y ];
*/
constexpr int8_t SRS_LUT[2][4][5][2] =
{
	// PIECE I
	{
		{{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}},      // UP
		{{-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2}},      // RIGHT
		{{-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0}},     // DOWN
		{{0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2}},       // LEFT
	},

	// OTHERS
	{
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},        // UP
		{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},       // RIGHT
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},        // DOWN
		{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},    // LEFT
	}
};

/*
* Combo table
* NOTE TO SELF: REN_LUT[state.ren]
*/
constexpr int MAX_COMBO_TABLE_SIZE = 12;
constexpr int REN_LUT[MAX_COMBO_TABLE_SIZE] =
{
	0,
	0,
	1,
	1,
	2,
	2,
	3,
	3,
	4,
	4,
	4,
	5
};