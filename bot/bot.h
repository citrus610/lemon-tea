#ifndef BOT_H
#define BOT_H

#include "beam.h"

struct bot_new_state {
	bitboard board;
	piece_type current = PIECE_NONE;
	piece_type hold = PIECE_NONE;
	piece_type next[16];
	int b2b = 0;
	int ren = 0;
};

struct bot_solution {
	// Bot's placement and instrcution
	piece_data placement;
	move move_list[32];
	int move_list_count = 0;
	bool is_hold = false;

	// Bot's log
	int node = 0;
	int depth = 0;
};

class bot
{
	std::mutex mutex;
	std::condition_variable cv;
	bool solution_ready = false;
	bool solution_need = false;
	bool running = false;
	std::vector<bot_new_state> new_state_buffer;
	std::vector<bot_solution> solution_buffer;

public:
	std::thread* thread = nullptr;

public:
	void start(int depth, weight heuristic, bool forecast);
	void destroy();
	bot_solution request_solution();
	void set_state(bot_new_state new_state);
	bool is_running();
};

#endif // BOT_H