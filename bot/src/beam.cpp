#include "beam.h"

void beam::init(int preview, bool is_forecast)
{
	// Setting things
	this->preview = preview;
	this->is_forecast = is_forecast;
	forecast_bag_index = 0;
	queue_count = 0;
	reward_count = 0;
	evaluator.heuristic.standard();

	// Make sure preview count is not too big
	this->preview = std::min(preview, max_depth);

	// If forecast mode on, then depth = max depth, else depth = preview count
	if (this->is_forecast)
		depth = max_depth;
	else
		depth = this->preview;

	// Init memory
	for (int i = 0; i < depth; ++i) {
		if (i == depth - 1)
			layer[i].init(1);
		else
			layer[i].init(max_node_per_layer);
	}
}

/*
* Reset the beam search
* Call this when we want to search from a new root state
*/
void beam::reset()
{
	// Setting things
	for (int i = 0; i < max_depth; ++i) {
		queue[i] = PIECE_NONE;
	}
	queue_count = preview;
	for (int i = 0; i < max_reward; ++i) {
		reward[i].visit = 0;
		reward[i].score = -999999;
	}
	reward_count = 0;

	// Clear vector
	for (int i = 0; i < depth; ++i) {
		layer[i].clear();
	}
}

/*
* Predict the future pieces according to the 7 bag rule
* NOTE TO SELF: Must call this in "set_root()" if forecast mode on
*/
void beam::forecast()
{
	// Init bag and shuffle bag randomly
	piece_type bag[7] = { PIECE_I, PIECE_T, PIECE_L, PIECE_O, PIECE_S, PIECE_J, PIECE_Z };
	for (int n = 0; n < 7; ++n) {
		int to_swap_index = rand() % 7;
		piece_type place_holder = bag[n];
		bag[n] = bag[to_swap_index];
		bag[to_swap_index] = place_holder;
	}

	// Find the number of piece remained from the previoud bag
	int number_of_piece_from_previous_bag = (forecast_bag_index + (preview % 7)) % 7;
	number_of_piece_from_previous_bag = std::min(number_of_piece_from_previous_bag, preview); // Make sure that it doesn't exceed preview count

	// Find piece that haven't appeared yet
	piece_type upcoming_piece[7] = {};
	int upcoming_piece_count = 0;
	for (int i = 0; i < 7; ++i) {
		bool already_appeared = false;
		for (int k = preview - number_of_piece_from_previous_bag; k < preview; ++k) {
			if (bag[i] == queue[k]) {
				already_appeared = true;
				break;
			}
		}
		if (!already_appeared) {
			upcoming_piece[upcoming_piece_count] = bag[i];
			upcoming_piece_count++;
		}
	}

	// Add upcoming piece to the queue
	memcpy(queue + preview, upcoming_piece, upcoming_piece_count * sizeof(piece_type));
	queue_count += upcoming_piece_count;

	// If the queue isn't full, then keep adding pieces from bag randomly
	while (true)
	{
		// Shuffle bag randomly
		for (int n = 0; n < 7; ++n) {
			int to_swap_index = rand() % 7;
			piece_type place_holder = bag[n];
			bag[n] = bag[to_swap_index];
			bag[to_swap_index] = place_holder;
		}

		// Keep adding piece to queue, if queue is full, then stop
		for (int i = 0; i < 7; ++i) {
			queue[queue_count] = bag[i];
			++queue_count;
			if (queue_count >= max_depth) break;
		}

		// If queue is full, then stop
		if (queue_count >= max_depth) break;
	}

	// Update bag index
	forecast_bag_index = (forecast_bag_index + 1) % 7;
}

/*
* NOTE TO SELF: Must call this after calling "reset()"
*/
void beam::set_root(bitboard& board, piece_type current, piece_type hold, piece_type next[max_depth], int b2b, int ren)
{
	this->reset(); // Not sure if this should be here
	node new_node;
	root = new_node;
	root.board = board;
	root.current = current;
	root.hold = hold;
	root.next = 0;
	root.b2b = b2b;
	root.ren = ren;
	queue_count = preview;
	memcpy(queue, next, preview * sizeof(piece_type));
	if (is_forecast) forecast();
}

void beam::expand_node(node& parent, vec<node>& new_layer, const bool& is_first, const bool& is_last, int& node_count)
{
	// Sanity check
	if (parent.current == PIECE_NONE) return;

	// For current piece
	piece_data current_piece_pos[MAX_MOVE_GEN_COUNT]; int current_piece_pos_count = 0;
	genmove::generate(parent.board, parent.current, current_piece_pos, current_piece_pos_count);
	node_count += current_piece_pos_count;
	if (is_last) {
		for (int i = 0; i < current_piece_pos_count; ++i) {
			node child;
			child.attemp(parent, current_piece_pos[i], false, queue, queue_count);
			child.score = evaluator.evaluate(child);
			if (new_layer.size == 0) { 
				new_layer.pointer[0] = child;
				++new_layer.size;
			}
			if (child.score > new_layer.pointer[0].score) new_layer.pointer[0] = child;
		}
	}
	else {
		for (int i = 0; i < current_piece_pos_count; ++i) {
			++new_layer.size;
			new_layer.pointer[new_layer.size - 1].attemp(parent, current_piece_pos[i], false, queue, queue_count);
			new_layer.pointer[new_layer.size - 1].score = evaluator.evaluate(new_layer.pointer[new_layer.size - 1]);
			if (is_first) { 
				new_layer.pointer[new_layer.size - 1].org_index = reward_count;
				reward[reward_count].score = new_layer.pointer[new_layer.size - 1].score;
				reward[reward_count].placement = current_piece_pos[i];
				++reward_count;
			}
			std::push_heap(new_layer.pointer, new_layer.pointer + new_layer.size);
		}
	}

	// For hold piece
	if (parent.hold == PIECE_NONE) {
		if (parent.next < queue_count) {
			piece_data hold_piece_pos[MAX_MOVE_GEN_COUNT]; int hold_piece_pos_count = 0;
			genmove::generate(parent.board, queue[parent.next], hold_piece_pos, hold_piece_pos_count);
			node_count += hold_piece_pos_count;
			if (is_last) {
				for (int i = 0; i < hold_piece_pos_count; ++i) {
					node child;
					child.attemp(parent, hold_piece_pos[i], true, queue, queue_count);
					child.score = evaluator.evaluate(child);
					if (new_layer.size == 0) {
						new_layer.pointer[0] = child;
						++new_layer.size;
					}
					if (child.score > new_layer.pointer[0].score) new_layer.pointer[0] = child;
				}
			}
			else {
				for (int i = 0; i < hold_piece_pos_count; ++i) {
					++new_layer.size;
					new_layer.pointer[new_layer.size - 1].attemp(parent, hold_piece_pos[i], true, queue, queue_count);
					new_layer.pointer[new_layer.size - 1].score = evaluator.evaluate(new_layer.pointer[new_layer.size - 1]);
					if (is_first) {
						new_layer.pointer[new_layer.size - 1].org_index = reward_count;
						reward[reward_count].score = new_layer.pointer[new_layer.size - 1].score;
						reward[reward_count].placement = hold_piece_pos[i];
						++reward_count;
					}
					std::push_heap(new_layer.pointer, new_layer.pointer + new_layer.size);
				}
			}
		}
	}
	else {
		piece_data hold_piece_pos[MAX_MOVE_GEN_COUNT]; int hold_piece_pos_count = 0;
		genmove::generate(parent.board, parent.hold, hold_piece_pos, hold_piece_pos_count);
		node_count += hold_piece_pos_count;
		if (is_last) {
			for (int i = 0; i < hold_piece_pos_count; ++i) {
				node child;
				child.attemp(parent, hold_piece_pos[i], true, queue, queue_count);
				child.score = evaluator.evaluate(child);
				if (new_layer.size == 0) {
					new_layer.pointer[0] = child;
					++new_layer.size;
				}
				if (child.score > new_layer.pointer[0].score) new_layer.pointer[0] = child;
			}
		}
		else {
			for (int i = 0; i < hold_piece_pos_count; ++i) {
				++new_layer.size;
				new_layer.pointer[new_layer.size - 1].attemp(parent, hold_piece_pos[i], true, queue, queue_count);
				new_layer.pointer[new_layer.size - 1].score = evaluator.evaluate(new_layer.pointer[new_layer.size - 1]);
				if (is_first) {
					new_layer.pointer[new_layer.size - 1].org_index = reward_count;
					reward[reward_count].score = new_layer.pointer[new_layer.size - 1].score;
					reward[reward_count].placement = hold_piece_pos[i];
					++reward_count;
				}
				std::push_heap(new_layer.pointer, new_layer.pointer + new_layer.size);
			}
		}
	}
}

void beam::expand(vec<node>& pre_layer, vec<node>& new_layer, const int& beam_width, const bool& is_last, int& node_count)
{
	// Expand by pop heap
	int max_pop = std::min(beam_width, pre_layer.size);
	for (int i = 0; i < max_pop; ++i) {
		// Expand beam_width best nodes
		expand_node(pre_layer.pointer[0], new_layer, false, is_last, node_count);

		// Accumulate reward
		++reward[pre_layer.pointer[0].org_index].visit;
		if (layer[depth - 1].size > 0) ++reward[layer[depth - 1].pointer[0].org_index].visit;

		// Pop heap
		std::pop_heap(pre_layer.pointer, pre_layer.pointer + pre_layer.size);
		pre_layer.pop();
	}

	// Discard the remaining useless nodes
	if (pre_layer.size < prune) pre_layer.clear();
}

int beam::search(int iteration)
{
	int iter_num = 0;
	int layer_index = 0;
	int node_count = 0;

	while (iter_num < iteration) {
		search_one_iter(iter_num, layer_index, node_count);
		++iter_num;
	}

	return node_count;
}

void beam::search_one_iter(int& iter_num, int& layer_index, int& node_count)
{
	int search_width = width_l;

	// Force 1st level search
	if (iter_num == 0) {
		expand_node(root, layer[0], true, false, node_count);
		layer_index = 0;
		return;
	}
	
	// Normal beam search
	if (iter_num < depth) {
		search_width = width_l;
		if (iter_num == 1) search_width = layer[0].size * (100 - prune_percentage) / 100;
		layer_index = iter_num - 1;
	}
	// Backtracking
	else {
		search_width = width_s;
		++layer_index;
		if (layer_index >= depth - 1) {
			layer_index = depth - 2;
			while (layer[layer_index].size == 0)
			{
				if (layer_index == 0) break;
				--layer_index;
			}
			if (layer_index == 0) return;
		}
	}

	// Expand
	expand(layer[layer_index], layer[layer_index + 1], search_width, (layer_index == depth - 2), node_count);
}

int beam::get_best_index()
{
	int result = 0;
	for (int i = 1; i < reward_count; ++i) {
		if (reward[result] < reward[i]) {
			result = i;
		}
	}
	return result;
}
