#include "botpool.h"

/*

BotPool::BotPool()
{
	for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
		state_buffer[i].clear();
		action_buffer[i].clear();
		solution_buffer[i].clear();
		this->thread[i] = nullptr;
		running[i] = false;
	}
}

void BotPool::init_thread(BotSetting setting, BotState state)
{
	// Check if the thread had already started
	if (this->is_running()) return;

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

void BotPool::end_thread()
{
	if (!this->is_running()) return;
	{
		std::unique_lock<std::mutex> lk(mutex);
		for (int i = 0; i < MAX_THREAD_COUNT; ++i)
			running[i] = false;
	}
	for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
		this->thread[i]->join();
		delete this->thread[i];
		this->thread[i] = nullptr;
		state_buffer[i].clear();
		action_buffer[i].clear();
		solution_buffer[i].clear();
	}
	std::cout << "destroy thread" << std::endl;
}

void BotPool::set_state(BotState state)
{
	if (!this->is_running()) return;

	std::unique_lock<std::mutex> lk(mutex);
	for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
		state_buffer[i].push_back(state);
	}
};

void BotPool::advance_state(BotAction action)
{
	if (!this->is_running()) return;

	std::unique_lock<std::mutex> lk(mutex);
	for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
		action_buffer[i].push_back(action);
	}
};

BotSolution BotPool::request_solution()
{
	std::unique_lock<std::mutex> lk(mutex);
	cv.wait(lk, [&]() {
		int solution_count = 0;
		for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
			solution_count += (int)(!solution_buffer[i].empty());
		}
		return solution_count > 0;
		});
	BotSolution result;
	for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
		if (!solution_buffer[i].empty())
			result = solution_buffer[i][0];
	}
	PathFinder::search(result.original_board, result.action.placement, result.move, result.move_count);
	return result;
};

bool BotPool::is_running()
{
	std::unique_lock<std::mutex> lk(mutex);
	int running_count = 0;
	for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
		running_count += (int)running[i];
	}
	return running_count > 0;
};

*/