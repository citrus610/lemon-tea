#include "bot.h"

void bot::start(int depth, weight heuristic, bool forecast)
{
	{
		std::unique_lock<std::mutex> lk(mutex);
		if (running) return;
	}

	std::cout << "start thread" << std::endl;

	solution_ready = false;
	solution_need = false;
	running = true;

	this->thread = new std::thread([&](int bot_depth, weight bot_heuristic, bool bot_forecast) {
		beam bot_beam;
		bot_beam.init(bot_depth, bot_forecast);
		bot_beam.evaluator.heuristic = bot_heuristic;

		int iter_num = 0;
		int layer_index = 0;
		int node_count = 0;
		int depth = 0;

		while (true)
		{
			bot_beam.search_one_iter(iter_num, layer_index, node_count);
			++iter_num;
			++depth;
			depth = std::min(depth, bot_beam.depth);

			std::unique_lock<std::mutex> lk(mutex);
			if (solution_need) {
				solution_need = false;

				bot_solution new_solution;
				new_solution.placement = bot_beam.reward[bot_beam.get_best_index()].placement;
				new_solution.is_hold = (new_solution.placement.type != bot_beam.root.current);
				new_solution.node = node_count;
				new_solution.depth = depth;
				pathfinder::search(bot_beam.root.board, new_solution.placement, new_solution.move_list, new_solution.move_list_count);
				new_solution.board = bot_beam.root.board;
				new_solution.visit = bot_beam.reward[bot_beam.get_best_index()].visit;
				new_solution.score = bot_beam.reward[bot_beam.get_best_index()].score;
				new_solution.index = bot_beam.get_best_index();

				solution_buffer.clear();
				solution_buffer.push_back(new_solution);
				solution_ready = true;
				lk.unlock();
				cv.notify_one();
				lk.lock();
			}
			if (!new_state_buffer.empty()) {
				iter_num = 0;
				layer_index = 0;
				node_count = 0;
				depth = 0;

				// If first hold then update forecast bag index
				if (new_state_buffer[0].first_hold) bot_beam.forecast_bag_index = (bot_beam.forecast_bag_index + 1) % 7;
				bot_beam.set_root(new_state_buffer[0].board, new_state_buffer[0].current, new_state_buffer[0].hold, new_state_buffer[0].next, new_state_buffer[0].b2b, new_state_buffer[0].ren);

				new_state_buffer.clear();
				solution_buffer.clear();
				solution_ready = false;
			}
			if (!running) {
				break;
			}
		}

		}, depth, heuristic, forecast);
}

void bot::destroy()
{
	std::unique_lock<std::mutex> lk(mutex);
	if (!running) return;
	running = false;
	solution_ready = false;
	solution_need = false;
	lk.unlock();
	this->thread->join();
	delete this->thread;
	this->thread = nullptr;
	std::cout << "destroy thread" << std::endl;
}

bot_solution bot::request_solution()
{
	std::unique_lock<std::mutex> lk(mutex);
	solution_need = true;
	lk.unlock();

	lk.lock();
	cv.wait(lk, [&]() { return solution_ready; });
	bot_solution result = solution_buffer[0];
	solution_buffer.clear();
	solution_ready = false;
	return result;
}

void bot::set_state(bot_new_state new_state)
{
	std::unique_lock<std::mutex> lk(mutex);
	new_state_buffer.push_back(new_state);
}

bool bot::is_running()
{
	std::unique_lock<std::mutex> lk(mutex);
	return running;
}