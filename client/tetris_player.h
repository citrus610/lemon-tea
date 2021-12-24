#pragma once

#include "type.h"

namespace TetrisGame
{

constexpr int PLAYER_DELAY_MOVE = 1;
constexpr int PLAYER_DELAY_LINE_CLEAR = 16;
constexpr int PLAYER_NEXT_COUNT = 5;
constexpr int PLAYER_PRE_GENERATE_BAG_SIZE = 7000;

class TetrisPlayer
{
public:
	BitBoard board;
	PieceType current = PIECE_NONE;
	PieceType hold = PIECE_NONE;
	std::vector<PieceType> next;
	int b2b = 0;
	int ren = 0;
public:
	PieceData piece;
	bool piece_locked = false;
public:
	std::vector<PieceType> bag;
	int bag_index = 0;
	TetrisPlayer* enemy = nullptr;
	int incomming_garbage = 0;
	int line_sent = 0;
public:
	int clearline_cnter = 0;
	int non_clearline_cnter = 0;
	bool gameover = false;
public:
	void init(std::vector<PieceType>& init_bag);
	void update();
public:
	void set_enemy(TetrisPlayer* other);
	void fill_queue();
	void update_current();
public:
	void do_hold();
	void do_right();
	void do_left();
	void do_rotate(bool cw);
	void do_down();
	void do_drop();
public:
	int count_garbage();
	void place_garbage();
	void send_garbage(int line);
	int count_fullline();
public:
	void update_clearline();
	bool is_clearline();
	bool just_clearline();
	void start_clearline();
};

};