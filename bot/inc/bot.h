#pragma once

#include "tree.h"
#include "debug.h"

struct BotSolution {
	BitBoard original_board;
	Action action;
	MoveType move[32];
	int move_count = 0;
	int node = 0;
	int depth = 0;
};

struct BotState {
	BitBoard board;
	PieceType current = PIECE_NONE;
	PieceType hold = PIECE_NONE;
	PieceType next[MAX_TREE_QUEUE];
	int next_count = 0;
	int b2b = 0;
	int ren = 0;
};

struct BotAction {
	Action action;
	PieceType new_piece[MAX_TREE_QUEUE];
	int new_piece_count = 0;
};

struct BotSetting {
	Weight weight;
	bool forecast = false;
};

class Bot
{
	std::mutex mutex;
	std::condition_variable cv;
	std::vector<BotState> state_buffer;
	std::vector<BotAction> action_buffer;
	std::vector<BotSolution> solution_buffer;
	bool running = false;
	bool solution_need = false;
public:
	std::thread* thread = nullptr;
public:
	void init_thread(BotSetting setting, BotState state);
	void end_thread();
	void set_state(BotState state);
	void advance_state(BotAction action);
	BotSolution request_solution();
	bool is_running();
};

