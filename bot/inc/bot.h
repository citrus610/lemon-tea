#pragma once

#include "tree.h"
#include "debug.h"

struct BotCandidate {
	Best best;
	Node root;
	int node = 0;
	int depth = 0;
};

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
	std::vector<BotState> state_buffer;
	std::vector<BotAction> action_buffer;
	std::vector<BotCandidate> candidate_buffer;
	bool running = false;
public:
	std::thread* thread = nullptr;
public:
	void init_thread(BotSetting setting, PieceType current, PieceType queue[MAX_TREE_QUEUE], int queue_count);
	void end_thread();
	void reset_state(BitBoard board, int b2b, int ren);
	void advance_state(BotAction action);
	bool request_solution(BotSolution& solution, int incomming_attack);
	bool is_running();
};

