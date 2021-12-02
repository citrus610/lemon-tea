#include "bot.h"

/*
* Start a bot thread
*/
void Bot::init_thread(BotSetting setting, PieceType current, PieceType queue[MAX_TREE_QUEUE], int queue_count)
{
	// Check if the thread had already started
	{
		std::unique_lock<std::mutex> lk(mutex);
		if (running) return;
	}

	// Reset thread data
	running = true;
	candidate_buffer.clear();
	state_buffer.clear();
	action_buffer.clear();

	// Init bot state
	BotState state;
	state.current = current;
	memcpy(state.next, queue, std::min(queue_count, MAX_TREE_QUEUE) * sizeof(PieceType));
	state.next_count = std::min(queue_count, MAX_TREE_QUEUE);

	// Start the thread
	this->thread = new std::thread([&](BotSetting bot_setting, BotState bot_state)
		{
			// Init tree
			Tree tree;
			tree.init();
			tree.set(
				BitBoard(),
				bot_state.current,
				PIECE_NONE,
				bot_state.next,
				bot_state.next_count,
				0,
				0
			);
			tree.evaluator.weight = bot_setting.weight;

			int iter_num = 0;
			int layer_index = 0;
			int width = 0;
			int node_count = 0;
			int depth = 0;

			while (true)
			{
				// Search one iter
				tree.search_one_iter(iter_num, layer_index, width, node_count);
				++iter_num;
				++depth;
				depth = std::max(depth, layer_index + 1);

				std::unique_lock<std::mutex> lk(mutex);

				// Get candidate list
				if (state_buffer.empty() && action_buffer.empty()) {
					BotCandidate new_candidate;
					new_candidate.best = tree.best;
					new_candidate.root = tree.root;
					new_candidate.node = node_count;
					new_candidate.depth = depth;

					if (candidate_buffer.empty()) candidate_buffer.push_back(new_candidate);
					candidate_buffer[0] = new_candidate;
				}

				// Set new game state
				if (!state_buffer.empty()) {
					iter_num = 0;
					layer_index = 0;
					width = 0;
					node_count = 0;
					depth = 0;

					PieceType queue_copy[MAX_TREE_QUEUE];
					memcpy(queue_copy, tree.queue, tree.queue_count * sizeof(PieceType));

					tree.set(
						state_buffer[0].board,
						tree.root.state.current,
						tree.root.state.hold,
						queue_copy,
						tree.queue_count,
						state_buffer[0].b2b,
						state_buffer[0].ren
					);

					state_buffer.clear();
				}

				// Advance the tree
				if (!action_buffer.empty()) {

					bool success = tree.advance(
						action_buffer[0].action,
						action_buffer[0].new_piece,
						action_buffer[0].new_piece_count
					);

					if (success) {
						iter_num = 0;
						layer_index = 0;
						width = 0;
						node_count = 0;
						depth = 0;
					}

					action_buffer.clear();
				}

				// Break if not running
				if (!running) break;
			}
		}, setting, state);

	std::cout << "start thread" << std::endl;
}

/*
* Destroy the bot thread
*/
void Bot::end_thread()
{
	{
		std::unique_lock<std::mutex> lk(mutex);
		if (!running) return;
		running = false;
	}
	this->thread->join();
	delete this->thread;
	this->thread = nullptr;
	candidate_buffer.clear();
	state_buffer.clear();
	action_buffer.clear();
	std::cout << "destroy thread" << std::endl;
}

/*
* Set a new board state for the bot
* Call this when misdrop of garbage received
*/
void Bot::reset_state(BitBoard board, int b2b, int ren)
{
	// Init new state
	BotState state;
	state.board = board;
	state.b2b = b2b;
	state.ren = ren;

	// Send state
	std::unique_lock<std::mutex> lk(mutex);
	if (!running) return;
	state_buffer.push_back(state);
	candidate_buffer.clear();
};

/*
* Tell the board to play a move and advance the tree's state
*/
void Bot::advance_state(BotAction action)
{
	std::unique_lock<std::mutex> lk(mutex);
	if (!running) return;
	action_buffer.push_back(action);
	candidate_buffer.clear();
};

/*
* Request the currently best solution
* Return true if new solution is found
*/
bool Bot::request_solution(BotSolution& solution, int incomming_attack)
{
	BotCandidate bot_candidate;
	{
		std::unique_lock<std::mutex> lk(mutex);
		if (candidate_buffer.empty()) return false;
		bot_candidate = candidate_buffer[0];
	}
	Node best_node = Tree::pick_from_best(bot_candidate.root, bot_candidate.best, incomming_attack);
	solution.original_board = bot_candidate.root.state.board;
	solution.action = best_node.origin;
	solution.node = bot_candidate.node;
	solution.depth = bot_candidate.depth;
	PathFinder::search(solution.original_board, solution.action.placement, solution.move, solution.move_count);
	return true;
};

/*
* Check if the bot thread is running
*/
bool Bot::is_running()
{
	std::unique_lock<std::mutex> lk(mutex);
	return running;
};
