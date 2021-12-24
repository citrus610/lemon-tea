#include "tree.h"

// Clear tree's layers
// Doesn't release memory
void Tree::clear()
{
	for (int i = 0; i < MAX_TREE_QUEUE; ++i) {
		this->layer[i].clear();
	}
}

// Init the tree's memory and initial state
void Tree::init(PieceType* init_queue, int queue_count)
{
    // Init setting
    this->queue.clear();
    this->bag.clear();
    this->root = Node();
    this->best = Best();
    this->evaluator.weight = DEFAULT_WEIGHT();

    // Init memory
    for (int i = 0; i < MAX_TREE_QUEUE; ++i) {
		this->layer[i].init((int)INIT_TREE_LAYER_GROUP[std::min(i, 3)]);
	}
	this->clear();

    // Set root state
    this->root.state = {
		.board = BitBoard(),
		.current = init_queue[0],
		.hold = PIECE_NONE,
		.next = 0,
		.b2b = 0,
		.ren = 0
	};

    // Set queue
    for (int i = 0; i < std::min(MAX_TREE_QUEUE, queue_count); ++i) {
        this->queue.add(init_queue[i]);
    }

    // Set bag
    PieceType full_bag[7] = { PIECE_I, PIECE_J, PIECE_L, PIECE_O, PIECE_S, PIECE_T, PIECE_Z };
    this->bag.clear();
    int start_index = this->queue.get_size() - (this->queue.get_size() % 7);
    for (int i = 0; i < 7; ++i) {
        bool exist_in_queue = false;
        for (int queue_index = start_index; queue_index < this->queue.get_size(); ++queue_index) {
            if (this->queue[queue_index] == full_bag[i]) {
                exist_in_queue = true;
                break;
            }
        }
        if (!exist_in_queue) {
            this->bag.add(full_bag[i]);
        }
    }
    this->queue.erase(0);
};

// Advance the root state of the tree
// Call this after we acquire the best move and we want to play that move
bool Tree::advance(Action action, PieceType* new_piece, int new_piece_count)
{
	// Check if action is valid
	if (action.placement.type == PIECE_NONE ||
		this->root.state.board.is_colliding(action.placement.x, action.placement.y, action.placement.type, action.placement.rotation) ||
		this->root.state.board.get_drop_distance(action.placement) != 0) {
		return false;
	}

    // Check if new pieces are valid
    arrayvec<PieceType, 7> bag_copy = this->bag;
    bool new_piece_valid = true;
    for (int new_piece_index = 0; new_piece_index < new_piece_count; ++new_piece_index) {
        bool valid = false;
        for (int bag_index = 0; bag_index < bag_copy.get_size(); ++bag_index) {
            if (bag_copy[bag_index] == new_piece[new_piece_index]) {
                valid = true;
                bag_copy.erase(bag_index);
                if (bag_copy.get_size() == 0) {
                    for (int i = 0; i < 7; ++i) bag_copy.add((PieceType)i);
                }
                break;
            } 
        }
        if (!valid) {
            new_piece_valid = false;
            break;
        }
    }
    if (!new_piece_valid) {
        return false;
    }

	// Make root node try the path
	Node previous_root = this->root;
	this->attempt_node(previous_root, this->root, action.placement, action.hold);
	this->evaluator.evaluate(this->root, this->queue.iter_begin(), this->queue.get_size());
	this->root.score.attack -= previous_root.score.attack;
	this->root.score.defence = 0;
	int queue_shift_value = this->root.state.next;
	this->root.state.next = 0;
	this->root.origin = Action();

	// Update queue & bag
    for (int i = 0; i < queue_shift_value; ++i) {
		this->queue.erase(0);
	}
    for (int i = 0; i < new_piece_count; ++i) {
        this->queue.add(new_piece[i]);
    }
    this->bag = bag_copy;

	// Reset best node
	this->best = Best();

	// Clear layer
	this->clear();

	return true;
}

// Reset the tree state
// Only call this after you received garbage or misdroped
void Tree::reset(BitBoard board, int b2b, int ren)
{
    // Reset root state
    this->root.state.board = board;
    this->root.state.b2b = b2b;
    this->root.state.ren = ren;

    // Reset best node
	this->best = Best();

	// Clear layer
	this->clear();
};

// Create new child from a parent by making the parent do the action
void Tree::attempt_node(Node& parent, Node& child, PieceData& placement, bool hold)
{
    // Setting child
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
	if (child.state.next < this->queue.get_size()) {
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
			child.action.lock = (LockType)((int)LOCK_TSPIN_1 + (line_cleared - 1));
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
				child.action.lock = (LockType)((int)LOCK_CLEAR_1 + (line_cleared - 1));
			}
		}
	}
	else {
		child.state.ren = 0;
		child.action.lock = LOCK_NONE;
	}
}

// Create new nodes from a parent then push them to a layer
void Tree::expand_node(Node& parent, Layer& new_layer, int& node_count)
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
		this->evaluator.evaluate(child, this->queue.iter_begin(), this->queue.get_size());
		if (parent.origin.placement.type == PIECE_NONE) child.origin = child.action;
		if (this->best.node < child) this->best.node = child;
		if (child.state.current != PIECE_NONE) new_layer.add(child);
	}
	node_count += current_list_count;

	// Expand hold piece
	if (parent.state.hold != parent.state.current) {
		// Find the hold piece to expand
		PieceType real_hold_piece = parent.state.hold;
		if (parent.state.hold == PIECE_NONE && parent.state.next < this->queue.get_size())
			real_hold_piece = this->queue[parent.state.next];

		// Sanity check
		if (real_hold_piece == PIECE_NONE) return;

		PieceData hold_list[MAX_MOVE_GENERATOR];
		int hold_list_count = 0;
		MoveGenerator::generate(parent.state.board, real_hold_piece, hold_list, hold_list_count);
		for (int i = 0; i < hold_list_count; ++i) {
			Node child;
			this->attempt_node(parent, child, hold_list[i], true);
			this->evaluator.evaluate(child, this->queue.iter_begin(), this->queue.get_size());
			if (parent.origin.placement.type == PIECE_NONE) child.origin = child.action;
			if (this->best.node < child) this->best.node = child;
			if (child.state.current != PIECE_NONE) new_layer.add(child);
		}
		node_count += hold_list_count;
	}
}

// Expand 'width' best nodes of the previous layer to the next layer
// Beam search
void Tree::expand_layer(Layer& previous_layer, Layer& new_layer, int& width, int& node_count)
{
	int max_pop = std::min(width, previous_layer.get_size());
	for (int i = 0; i < max_pop; ++i) {
		// Get best group index
		int best_group_index = previous_layer.max_group();

		// Expand the best node
		this->expand_node(previous_layer.data[best_group_index][0], new_layer, node_count);

		// Pop 
		previous_layer.pop(best_group_index);
	}
}

// Do a search with N iterations, then return node count
int Tree::search(int iteration)
{
	int iter_num = 0;
	int layer_index = 0;
	int node_count = 0;

	while (iter_num < iteration) {
		think(iter_num, layer_index, node_count);
		++iter_num;
	}

	return node_count;
}

// Do one iteration of the search
void Tree::think(int& iter_num, int& layer_index, int& node_count)
{
	// Force level 1 search
	if (iter_num == 0) {
		expand_node(this->root, this->layer[0], node_count);
		if (this->layer[0].data[0].get_size() > 0) {
			this->best.node = this->layer[0].data[0][0];
			for (int i = 0; i < this->layer[0].data[0].get_size(); ++i) {
				if (this->best.node < this->layer[0].data[0][i]) this->best.node = this->layer[0].data[0][i];
				int node_spike = Evaluator::spike(this->root, this->layer[0].data[0][i]);
				if (node_spike > this->best.spike_count) {
					this->best.spike = this->layer[0].data[0][i];
					this->best.spike_count = node_spike;
				}
			}
		}
		layer_index = 0;
		return;
	}

	// Check if layer empty, then go down
	if (this->layer[layer_index].get_size() == 0) layer_index = 0;
	while (this->layer[layer_index].get_size() == 0) {
		if (layer_index + 1 < MAX_TREE_QUEUE) {
			++layer_index;
        }
        else {
			return;
        }
	}

	// Expand the layer
	expand_layer(this->layer[layer_index], this->layer[layer_index + 1], this->beam, node_count);

	// Update layer index
	++layer_index;
	if (layer_index >= MAX_TREE_QUEUE - 1) {
		--layer_index;
		this->layer[MAX_TREE_QUEUE - 1].clear();
	}
}

// Get the (currently) best node
Node Tree::solution(int incomming_attack)
{
	return Tree::pick_from_best(this->root, this->best, incomming_attack);
}

// Pick the best move from a list of candidate with incomming garbage in mind
Node Tree::pick_from_best(Node& root, Best& candidate, int incomming_attack)
{
	// Let the result be the best node
	Node result = candidate.node;

	// If there is incomming garbage and the bot is about to die, let result be the best spike action
	int root_height[10];
	root.state.board.get_height(root_height);
	int max_height_center = *std::max_element(root_height + 3, root_height + 7);
	if (candidate.spike_count > 0 &&
		incomming_attack > 0 &&
		max_height_center + incomming_attack - Evaluator::spike(root, candidate.node) > 20) {
		result = candidate.spike;
	}

	return result;
}