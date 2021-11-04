#include "bot.h"

/*
* Start a bot thread
*/
void Bot::init_thread(BotSetting setting, BotState state)
{
	// Check if the thread had already started
	{
		std::unique_lock<std::mutex> lk(mutex);
		if (running) return;
	}

	// Reset thread data
	running = true;
	solution_buffer.clear();
	state_buffer.clear();
	action_buffer.clear();

	// Start the thread
	this->thread = new std::thread([&](BotSetting bot_setting, BotState bot_state)
		{
			// Init tree
			Tree tree;
			tree.init();
			tree.set(
				bot_state.board,
				bot_state.current,
				bot_state.hold,
				bot_state.next,
				bot_state.next_count,
				bot_state.b2b,
				bot_state.ren
			);
			tree.evaluator.weight = bot_setting.weight;

			int iter_num = 0;
			int layer_index = 0;
			int node_count = 0;
			int depth = 0;

			while (true)
			{
				// Search one iter
				tree.search_one_iter(iter_num, layer_index, node_count);
				++iter_num;
				++depth;
				depth = std::min(depth, tree.queue_count + 1);

				// Create solution
				{
					std::unique_lock<std::mutex> lk(mutex);

					BotSolution new_solution;
					new_solution.original_board = tree.root.state.board;
					new_solution.action = tree.get_best().origin;
					new_solution.node = node_count;
					new_solution.depth = depth;

					if (solution_buffer.empty()) solution_buffer.push_back(new_solution);
					solution_buffer[0] = new_solution;
				}
				cv.notify_one();

				std::unique_lock<std::mutex> lk(mutex);

				// Set new game state
				if (!state_buffer.empty()) {
					iter_num = 0;
					layer_index = 0;
					node_count = 0;
					depth = 0;

					tree.set(
						state_buffer[0].board,
						state_buffer[0].current,
						state_buffer[0].hold,
						state_buffer[0].next,
						state_buffer[0].next_count,
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
	std::cout << "destroy thread" << std::endl;
}

/*
* Set a new game state for the bot
*/
void Bot::set_state(BotState state)
{
	std::unique_lock<std::mutex> lk(mutex);
	if (!running) return;
	state_buffer.push_back(state);
};

/*
* Tell the board to play a move and advance the tree
*/
void Bot::advance_state(BotAction action)
{
	std::unique_lock<std::mutex> lk(mutex);
	if (!running) return;
	action_buffer.push_back(action);
};

/*
* Request the currently best solution
*/
BotSolution Bot::request_solution()
{
	std::unique_lock<std::mutex> lk(mutex);
	cv.wait(lk, [&]() { return !solution_buffer.empty(); });
	BotSolution result = solution_buffer[0];
	PathFinder::search(result.original_board, result.action.placement, result.move, result.move_count);
	return result;
};

/*
* Check if the bot thread is running
*/
bool Bot::is_running()
{
	std::unique_lock<std::mutex> lk(mutex);
	return running;
};
