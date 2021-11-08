#include "tree.h"

/*
* Init the tree
* Must call when a new tree is created
*/
void Tree::init()
{
	this->queue_count = 0;

	this->evaluator.weight.standard();

	// Init memory
	for (int i = 0; i < MAX_TREE_QUEUE + 2; ++i) {
		this->layer[i].init(MAX_TREE_LAYER_SIZE);
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
}

/*
* Set a new root state of the tree
* Call this when reseting tree
*/
void Tree::set(BitBoard board, PieceType current, PieceType hold, PieceType next[MAX_TREE_QUEUE], int next_count, int b2b, int ren)
{
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
	this->best = Node();

	// Set new queue
	memcpy(this->queue, next, std::min(next_count, MAX_TREE_QUEUE) * sizeof(PieceType));
	this->queue_count = std::min(next_count, MAX_TREE_QUEUE);

	// Clear layer
	this->clear();
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
	// TODO .. check if action's piece type is valid
	// TODO .. if forecast on, check if new queue is valid

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
	this->best = Node();

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
		if (this->best < child) this->best = child;
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
			if (this->best < child) this->best = child;
			if (child.state.current != PIECE_NONE) new_layer.add(child);
		}
		node_count += hold_list_count;
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
			this->best = this->layer[0][0];
			for (int i = 1; i < this->layer[0].get_size(); ++i)
				if (this->best < this->layer[0][i]) this->best = this->layer[0][i];
		}
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
Node Tree::get_best()
{
	return this->best;
}
