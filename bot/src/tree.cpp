#include "tree.h"

/*
* Init the tree
* Must call when a new tree is created
*/
void Tree::init(bool forecast)
{
	this->queue_count = 0;
	this->forecast = forecast;
	this->evaluator.weight.standard();

	// Init memory
	for (int i = 0; i < MAX_TREE_QUEUE + 2; ++i) {
		this->layer[i].init(MAX_TREE_LAYER_SIZE);
	}
	if (this->forecast) {
		for (int i = 0; i < MAX_TREE_LAYER_FORECAST_DEPTH; ++i) {
			this->layer_forecast[i].init(MAX_TREE_LAYER_FORECAST_SIZE);
		}
	}
	this->clear();
}

/*
* Clear tree's layers
* Doesn't release memory
*/
void Tree::clear()
{
	for (int i = 0; i < MAX_TREE_QUEUE + 2; ++i) {
		this->layer[i].clear();
	}
	this->clear_forecast();
}

/*
* Clear tree's forecast layers
* Doesn't release memory
*/
void Tree::clear_forecast()
{
	for (int i = 0; i < MAX_TREE_LAYER_FORECAST_DEPTH; ++i) {
		this->layer_forecast[i].clear();
	}
}

/*
* Set a new root state of the tree
* Call this when reseting tree
*/
bool Tree::set(BitBoard board, PieceType current, PieceType hold, PieceType next[MAX_TREE_QUEUE], int next_count, int b2b, int ren, bool init)
{
	// Check valid for forecast mode
	if (this->forecast && (!init)) {
		if (next_count != this->queue_count) return false;
		for (int i = 0; i < this->queue_count; ++i)
			if (next[i] != this->queue[i]) return false;
	}

	// Reset root node
	this->root = Node();
	this->root.state = {
		board,
		current,
		hold,
		0,
		b2b,
		ren
	};

	// Reset best node
	this->best = Best();

	// Set new queue
	memcpy(this->queue, next, std::min(next_count, MAX_TREE_QUEUE) * sizeof(PieceType));
	this->queue_count = std::min(next_count, MAX_TREE_QUEUE);

	// Create bag if forecast
	if (this->forecast && init) {
		PieceType init_bag[7] = {
			PIECE_I,
			PIECE_J,
			PIECE_L,
			PIECE_S,
			PIECE_Z,
			PIECE_T,
			PIECE_O
		};

		PieceType init_queue[MAX_TREE_QUEUE * 2] = { current };
		int init_queue_count = this->queue_count + 1;
		memcpy(init_queue + 1, this->queue, this->queue_count * sizeof(PieceType));

		if (init_queue_count % 7 == 0) {
			memcpy(this->bag, init_bag, 7 * sizeof(PieceType));
		}
		else {
			for (int i = 0; i < 7; ++i) {
				bool exist = false;
				for (int k = init_queue_count - (init_queue_count % 7); k < init_queue_count; ++k) {
					if (init_bag[i] == init_queue[k]) {
						exist = true;
						break;
					}
				}
				if (!exist) {
					this->bag[this->bag_count] = init_bag[i];
					++this->bag_count;
				}
			}
		}
	}

	// Clear layer
	this->clear();

	return true;
}

/*
* Advance the root state of the tree
* Call this after we acquire the best move and we want to play that move
*/
bool Tree::advance(Action& action, PieceType new_piece[MAX_TREE_QUEUE], int new_piece_count)
{
	// Check if action is valid
	if (action.placement.type == PIECE_NONE ||
		this->root.state.board.is_colliding(action.placement.x, action.placement.y, action.placement.type, action.placement.rotation) ||
		this->root.state.board.get_drop_distance(action.placement) != 0) {
		return false;
	}
	if (action.hold) {
		if (this->root.state.hold == PIECE_NONE) {
			if (this->queue_count < 1 || action.placement.type != this->queue[0])
				return false;
		}
		else {
			if (action.placement.type != this->root.state.hold)
				return false;
		}
	}
	else {
		if (action.placement.type != this->root.state.current)
			return false;
	}

	// If forecast on, check if new pieces is valid
	if (this->forecast) {
		// Check if all new pieces are valid
		bool new_piece_valid = true;

		PieceType bag_copy[7];
		memcpy(bag_copy, this->bag, this->bag_count * sizeof(PieceType));
		int bag_copy_count = this->bag_count;

		// For every piece in the new piece queue
		for (int i = 0; i < new_piece_count; ++i) {
			// Check if valid piece
			bool piece_valid = false;
			int in_bag_index = -1;
			for (int k = 0; k < bag_copy_count; ++k) {
				if (bag_copy[k] == new_piece[i]) {
					piece_valid = true;
					in_bag_index = k;
					break;
				}
			}

			// If piece is valid then ok
			if (piece_valid) {
				for (int k = in_bag_index; k < bag_copy_count - 1; ++k) {
					bag_copy[k] = bag_copy[k + 1];
				}
				--bag_copy_count;
				if (bag_copy_count <= 0) {
					bag_copy_count = 7;
					bag_copy[0] = PIECE_I;
					bag_copy[1] = PIECE_J;
					bag_copy[2] = PIECE_L;
					bag_copy[3] = PIECE_S;
					bag_copy[4] = PIECE_Z;
					bag_copy[5] = PIECE_T;
					bag_copy[6] = PIECE_O;
				}
			}
			else {
				new_piece_valid = false;
				break;
			}
		}

		// If there is an invalid piece return false
		if (!new_piece_valid) return false;

		// Else update bag
		this->bag_count = 0;
		for (int i = 0; i < 7; ++i) {
			bool exist = false;
			for (int k = new_piece_count - (new_piece_count % 7); k < new_piece_count; ++k) {
				if (new_piece[k] == (PieceType)i) {
					exist = true;
					break;
				}
			}
			if (!exist) {
				this->bag[this->bag_count] = (PieceType)i;
				++this->bag_count;
			}
		}

		// If forecast bag is empty, refill it
		if (this->bag_count <= 0) {
			this->bag_count = 7;
			this->bag[0] = PIECE_I;
			this->bag[1] = PIECE_J;
			this->bag[2] = PIECE_L;
			this->bag[3] = PIECE_S;
			this->bag[4] = PIECE_Z;
			this->bag[5] = PIECE_T;
			this->bag[6] = PIECE_O;
		}
	}

	// Make root node try the path
	Node previous_root = this->root;
	this->attempt_node(previous_root, this->root, action.placement, action.hold);
	this->evaluator.evaluate(this->root, this->queue, this->queue_count);
	this->root.score.attack -= previous_root.score.attack;
	this->root.score.defence = 0;
	int queue_shift_value = this->root.state.next;
	this->root.state.next = 0;
	this->root.origin = Action();

	// Update queue
	for (int i = 0; i < this->queue_count - queue_shift_value; ++i) {
		this->queue[i] = this->queue[i + queue_shift_value];
	}
	this->queue_count -= queue_shift_value;
	int real_new_piece_count = std::min(new_piece_count, MAX_TREE_QUEUE - this->queue_count);
	memcpy(this->queue + this->queue_count, new_piece, real_new_piece_count * sizeof(PieceType));
	this->queue_count += real_new_piece_count;

	// Reset best node
	this->best = Best();

	// Clear layer
	this->clear();

	return true;
}

/*
* Create new child from a parent by making the parent do the action
*/
void Tree::attempt_node(Node& parent, Node& child, PieceData& placement, bool hold)
{
	child = parent;
	child.action.placement = placement;
	child.action.soft_drop = !parent.state.board.is_above_stack(placement);
	child.action.hold = hold;

	// Update hold piece
	if (hold) {
		if (child.state.hold == PIECE_NONE) {
			++child.state.next;
		}
		child.state.hold = child.state.current;
	}

	// Update current piece
	if (child.state.next < this->queue_count) {
		child.state.current = this->queue[child.state.next];
		++child.state.next;
	}
	else {
		child.state.current = PIECE_NONE;
	}

	// Check t spin, place piece and clear line
	bool is_t_spin = child.state.board.is_t_spin(placement);
	child.state.board.place_piece(placement);
	int line_cleared = child.state.board.clear_line();

	// Update ren, b2b and lock data
	if (line_cleared > 0) {
		++child.state.ren;
		// T spin
		if (is_t_spin) {
			++child.state.b2b;
			child.action.lock = (LockData)((int)LOCK_TSPIN_1 + (line_cleared - 1));
		}
		else {
			// Perfect clear
			if (child.state.board.column[0] == (uint64_t)0 &&
				child.state.board.column[1] == (uint64_t)0 &&
				child.state.board.column[2] == (uint64_t)0 &&
				child.state.board.column[3] == (uint64_t)0 &&
				child.state.board.column[4] == (uint64_t)0 &&
				child.state.board.column[5] == (uint64_t)0 &&
				child.state.board.column[6] == (uint64_t)0 &&
				child.state.board.column[7] == (uint64_t)0 &&
				child.state.board.column[8] == (uint64_t)0 &&
				child.state.board.column[9] == (uint64_t)0) {
				child.action.lock = LOCK_PC;
			}
			else {
				// Normal clear line
				if (line_cleared == 4)
					++child.state.b2b;
				else
					child.state.b2b = 0;
				child.action.lock = (LockData)((int)LOCK_CLEAR_1 + (line_cleared - 1));
			}
		}
	}
	else {
		child.state.ren = 0;
		child.action.lock = LOCK_NONE;
	}
}

/*
* Call this function when reach the end of the queue
* Convert normal node to forecast node then push it to the first forecast layer
* The normal node/parent node must has PIECE_NONE as current piece
*/
void Tree::convert_node_forecast(Node& parent, int& node_count)
{
	// Create new forecast node
	NodeForecast new_node_forecast;
	new_node_forecast.parent = parent;
	new_node_forecast.bag_count = this->bag_count;
	memcpy(new_node_forecast.bag, this->bag, this->bag_count * sizeof(PieceType));

	// Forecast the node base on the bag
	this->forecast_node(new_node_forecast, node_count);

	// Evaluate the forecast node
	this->evaluator.evaluate_forecast(new_node_forecast);

	// Add to the first forecast layer
	this->push_node_forecast(new_node_forecast, this->layer_forecast[0]);
}

/*
* Generate parallel universe :)
*/
void Tree::forecast_node(NodeForecast& node_forecast, int& node_count)
{
	// For every remaining piece in bag, find the best placement
	for (int piece_index = 0; piece_index < node_forecast.bag_count; ++piece_index) {
		// Copy parent;
		Node parent_copy = node_forecast.parent;
		parent_copy.state.current = node_forecast.bag[piece_index];

		// Expand parent to find the best child with this piece
		Node best_piece_child;

		// Expand current piece
		PieceData current_list[MAX_MOVE_GENERATOR];
		int current_list_count = 0;
		MoveGenerator::generate(parent_copy.state.board, parent_copy.state.current, current_list, current_list_count);
		for (int i = 0; i < current_list_count; ++i) {
			Node child;
			this->attempt_node(parent_copy, child, current_list[i], false);
			this->evaluator.evaluate(child, node_forecast.bag, node_forecast.bag_count);
			if (best_piece_child.origin.placement.type == PIECE_NONE) best_piece_child = child;
			if (best_piece_child < child) best_piece_child = child;
		}
		node_count += current_list_count;

		// Expand hold piece
		if (parent_copy.state.hold != parent_copy.state.current && parent_copy.state.hold != PIECE_NONE) {
			PieceData hold_list[MAX_MOVE_GENERATOR];
			int hold_list_count = 0;
			MoveGenerator::generate(parent_copy.state.board, parent_copy.state.hold, hold_list, hold_list_count);
			for (int i = 0; i < hold_list_count; ++i) {
				Node child;
				this->attempt_node(parent_copy, child, hold_list[i], true);
				this->evaluator.evaluate(child, node_forecast.bag, node_forecast.bag_count);
				if (best_piece_child.origin.placement.type == PIECE_NONE) best_piece_child = child;
				if (best_piece_child < child) best_piece_child = child;
			}
			node_count += hold_list_count;
		}

		// Add best piece child to children list
		node_forecast.children[piece_index] = best_piece_child;
	}
}

/*
* Push a forecast node to a forecast layer in a way that maintain the piority queue
*/
void Tree::push_node_forecast(NodeForecast& node, vec<NodeForecast>& new_layer)
{
	if (new_layer.get_size() < this->beam) {
		new_layer.add(node);
		if (new_layer.get_size() >= this->beam) {
			std::make_heap(
				new_layer.iter_begin(),
				new_layer.iter_end(),
				[&](NodeForecast& first, NodeForecast& second) { return second.score < first.score; }
			);
		}
	}
	else {
		if (node.score > new_layer[0].score) {
			std::pop_heap(
				new_layer.iter_begin(),
				new_layer.iter_end(),
				[&](NodeForecast& first, NodeForecast& second) { return second.score < first.score; }
			);
			new_layer.pop();
			new_layer.add(node);
			std::push_heap(
				new_layer.iter_begin(),
				new_layer.iter_end(),
				[&](NodeForecast& first, NodeForecast& second) { return second.score < first.score; }
			);
		}
	}
}

/*
* Create new nodes from a parent then push them to a layer
*/
void Tree::expand_node(Node& parent, vec<Node>& new_layer, int& node_count)
{
	// Sanity check
	if (parent.state.current == PIECE_NONE) return;

	// Expand current piece
	PieceData current_list[MAX_MOVE_GENERATOR];
	int current_list_count = 0;
	MoveGenerator::generate(parent.state.board, parent.state.current, current_list, current_list_count);
	for (int i = 0; i < current_list_count; ++i) {
		Node child;
		this->attempt_node(parent, child, current_list[i], false);
		this->evaluator.evaluate(child, this->queue, this->queue_count);
		if (parent.origin.placement.type == PIECE_NONE) child.origin = child.action;
		if (this->best.node < child) this->best.node = child;
		if (child.state.current != PIECE_NONE) new_layer.add(child);
	}
	node_count += current_list_count;

	// Expand hold piece
	if (parent.state.hold != parent.state.current) {
		// Find the hold piece to expand
		PieceType real_hold_piece = parent.state.hold;
		if (parent.state.hold == PIECE_NONE && parent.state.next < this->queue_count)
			real_hold_piece = this->queue[parent.state.next];

		// Sanity check
		if (real_hold_piece == PIECE_NONE) return;

		PieceData hold_list[MAX_MOVE_GENERATOR];
		int hold_list_count = 0;
		MoveGenerator::generate(parent.state.board, real_hold_piece, hold_list, hold_list_count);
		for (int i = 0; i < hold_list_count; ++i) {
			Node child;
			this->attempt_node(parent, child, hold_list[i], true);
			this->evaluator.evaluate(child, this->queue, this->queue_count);
			if (parent.origin.placement.type == PIECE_NONE) child.origin = child.action;
			if (this->best.node < child) this->best.node = child;
			if (child.state.current != PIECE_NONE) new_layer.add(child);
		}
		node_count += hold_list_count;
	}
}

/*
* The same as expand normal node
*/
void Tree::expand_node_forecast(NodeForecast& parent, vec<NodeForecast>& new_layer, int& node_count)
{
	// For every piece in bag
	for (int piece_index = 0; piece_index < parent.bag_count; ++piece_index) {
		// Create child node
		NodeForecast child;

		// Set parent
		child.parent = parent.children[piece_index];

		// Set bag
		if (parent.bag_count <= 1) {
			child.bag_count = 7;
			child.bag[0] = PIECE_I;
			child.bag[1] = PIECE_J;
			child.bag[2] = PIECE_L;
			child.bag[3] = PIECE_S;
			child.bag[4] = PIECE_Z;
			child.bag[5] = PIECE_T;
			child.bag[6] = PIECE_O;
		}
		else {
			for (int i = 0; i < parent.bag_count; ++i) {
				if (i != piece_index) {
					child.bag[child.bag_count] = parent.bag[i];
					++child.bag_count;
				}
			}
		}

		// Forecast
		this->forecast_node(child, node_count);

		// Evaluate
		this->evaluator.evaluate_forecast(child);

		// Add to next layer
		this->push_node_forecast(child, new_layer);
	}
}

/*
* Expand 'width' best nodes of the previous layer to the next layer
* Beam search
*/
void Tree::expand_layer(vec<Node>& previous_layer, vec<Node>& new_layer, int& width, int& node_count)
{
	int max_pop = std::min(width, previous_layer.get_size());
	for (int i = 0; i < max_pop; ++i) {
		// Expand the best node
		this->expand_node(previous_layer[0], new_layer, node_count);

		// Pop heap
		std::pop_heap(previous_layer.iter_begin(), previous_layer.iter_end());
		previous_layer.pop();
	}
	std::make_heap(new_layer.iter_begin(), new_layer.iter_end());
}

/*
* The same as normal expand layer
* Pure beam search, no back tracking
*/
void Tree::expand_layer_forecast(vec<NodeForecast>& previous_layer, vec<NodeForecast>& new_layer, int& width, int& node_count)
{
	int max_pop = std::min(width, previous_layer.get_size());
	for (int i = max_pop - 1; i >= 0; --i) {
		this->expand_node_forecast(previous_layer[i], new_layer, node_count);
	}
}

/*
* Do a search with N iterations, then return node count
*/
int Tree::search(int iteration)
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

/*
* Do one iteration of the search
*/
void Tree::search_one_iter(int& iter_num, int& layer_index, int& node_count)
{
	// Force level 1 search
	if (iter_num == 0) {
		expand_node(this->root, this->layer[0], node_count);
		if (this->layer[0].get_size() > 0) {
			this->best.node = this->layer[0][0];
			for (int i = 0; i < this->layer[0].get_size(); ++i) {
				if (this->best.node < this->layer[0][i]) this->best.node = this->layer[0][i];
				int node_spike = Evaluator::spike(this->root, this->layer[0][i]);
				if (node_spike > this->best.spike_count) {
					this->best.spike = this->layer[0][i];
					this->best.spike_count = node_spike;
				}
			}
		}
		std::make_heap(this->layer[0].iter_begin(), this->layer[0].iter_end());
		layer_index = 0;
		return;
	}

	// Check backtracking and find the layer to expand
	bool backtrack = false;
	while (this->layer[layer_index].get_size() == 0) {
		backtrack = true;
		if (layer_index > 0)
			--layer_index;
		else
			return;
	}

	// Expand the layer
	expand_layer(this->layer[layer_index], this->layer[layer_index + 1], this->beam, node_count);

	// If backtrack, then prune nodes
	if (backtrack) {
		if (this->layer[layer_index].get_size() < this->beam * 25) {
			this->layer[layer_index].clear();
		}
	}

	// Update layer index
	++layer_index;
	if (layer_index >= MAX_TREE_QUEUE - 1) {
		--layer_index;
		this->layer[MAX_TREE_QUEUE - 1].clear();
	}
}

/*
* Get the (currently) best node
*/
Node Tree::get_best(int incomming_attack)
{
	return Tree::pick_from_best(this->root, this->best, incomming_attack);
}

/*
* Pick the best move from a list of candidate with incomming garbage in mind
*/
Node Tree::pick_from_best(Node& root, Best& candidate, int incomming_attack)
{
	// Let the result be the best node
	Node result = candidate.node;

	// If forecast mode is on and the forecast node is better than the best node, let result be the forecast node
	if (candidate.forecast.parent.origin.placement.type != PIECE_NONE &&
		candidate.forecast.score > result.score.attack + result.score.defence) {
		result = candidate.forecast.parent;
	}

	// If there is incomming garbage and the bot is about to die, let result be the best spike action
	int root_height[10];
	root.state.board.get_height(root_height);
	int max_height_center = *std::max_element(root_height + 3, root_height + 7);
	if (candidate.spike_count > 0 &&
		incomming_attack > 0 &&
		max_height_center + incomming_attack - Evaluator::spike(root, result) > 20) {
		result = candidate.spike;
	}

	return result;
}
