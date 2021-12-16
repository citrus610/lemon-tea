#include "tree.h"

Tree::Tree()
{
    this->candidate = vec<Node>(false);
};

Tree::~Tree()
{
    this->destroy();
};

// Init the tree's memory and initial state
void Tree::init(PieceType* init_queue, int queue_count)
{
    // Init setting
    this->node_count = 0;
    this->depth = 0;
    this->bag.clear();
    this->root = NodeState();
    this->forecast = false;
    this->eval.weight = DEFAULT_WEIGHT();

    // Init memory
    this->pool.init(this->min_pool);

    // Set the first layer/candidate
    this->pool.get_chunk(this->candidate);

    // Set root state
    this->root = NodeState();
    for (int i = 0; i < std::min(queue_count, GAME_MAX_QUEUE); ++i) {
        this->root.state.queue.add(init_queue[i]);
    }

    // Set bag
    PieceType full_bag[7] = {
        PIECE_I,
        PIECE_J,
        PIECE_L,
        PIECE_O,
        PIECE_S,
        PIECE_T,
        PIECE_Z
    };
    this->bag.clear();
    int start_index = this->root.state.queue.get_size() - (this->root.state.queue.get_size() % 7);
    for (int i = 0; i < 7; ++i) {
        bool exist_in_queue = false;
        for (int queue_index = start_index; queue_index < this->root.state.queue.get_size(); ++queue_index) {
            if (this->root.state.queue[queue_index] == full_bag[i]) {
                exist_in_queue = true;
                break;
            }
        }
        if (!exist_in_queue) {
            this->bag.add(full_bag[i]);
        }
    }

    // Force first search
    this->expand(this->candidate, this->root);
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        this->candidate[i].parent = nullptr;
        NodeState candidate_state = this->root;
        this->attempt(candidate_state, this->candidate[i].placement);
        this->eval.evaluate(this->candidate[i], candidate_state);
    }

    // Set stats
    this->node_count = this->candidate.get_size();
    this->depth = 1;
};

// Reset the tree state
// Only call this after you received garbage or misdroped
void Tree::reset(BitBoard board, int b2b, int ren)
{
    // Reset root state
    this->root.state.board = board;
    this->root.state.b2b = b2b;
    this->root.state.ren = ren;

    // Release old nodes
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        this->release(this->candidate.iter_begin() + i);
    }

    // Force first search
    this->candidate.clear();
    this->expand(this->candidate, this->root);
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        this->candidate[i].parent = nullptr;
        NodeState candidate_state = this->root;
        this->attempt(candidate_state, this->candidate[i].placement);
        this->eval.evaluate(this->candidate[i], candidate_state);
    }

    // Reset stats
    this->node_count = this->candidate.get_size();
    this->depth = 1;
};

// Advance the state of the tree
// Making the tree play the move and add new pieces to the queue
bool Tree::advance(PieceData placement, PieceType* new_piece, int new_piece_count)
{
    // Find the next branch
    int branch_index = -1;
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        if (this->candidate[i].placement == placement) {
            branch_index = i;
            break;
        }
    }

    // Check if placement is valid
    if (branch_index == -1) {
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
                    bag_copy.add(PIECE_I);
                    bag_copy.add(PIECE_J);
                    bag_copy.add(PIECE_L);
                    bag_copy.add(PIECE_O);
                    bag_copy.add(PIECE_Z);
                    bag_copy.add(PIECE_T);
                    bag_copy.add(PIECE_S);
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

    // Update bag and queue
    this->bag = bag_copy;
    for (int i = 0; i < new_piece_count; ++i) {
        this->root.state.queue.add(new_piece[i]);
    }

    // Update tree's status
    this->node_count -= this->candidate.get_size();
    this->depth -= 1;

    // Discard the other branches
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        if (i != branch_index) {
            this->release(this->candidate.iter_begin() + i);
        }
    }

    // Advance root state
    this->attempt(this->root, placement);

    // Update the candidate list
    vec<Node> placeholder = this->candidate[branch_index].children;
    this->pool.return_chunk(this->candidate);
    this->candidate = placeholder;
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        this->candidate[i].parent = nullptr;
    }

    return true;
};

// Destroy the tree when you don't want to search anymore
// Don't have to call this function, since the tree's destructor automatically do that
// Calling this many times should be safe ... i think ... UB
void Tree::destroy()
{
    // Sanity check
    if (this->candidate.iter_begin() == nullptr) return;

    // Release every branches
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        this->release(this->candidate.iter_begin() + i);
    }

    // Return the candidate chunk to the pool
    this->pool.return_chunk(this->candidate);
};

// Function return the random distribution for selection based on rank
int Tree::distribution(int x)
{
    return 16667 / (x * x);
};

// Choose a node index to explore
int Tree::select(vec<Node>& layer)
{
    // Sanity check
    if (layer.get_size() == 0) {
        return -1;
    }

    // Find the rank by weighted random
    int sum_of_weight = 0;
    int score[CHUNK_SIZE];
    for (int i = 0; i < layer.get_size(); ++i) {
        sum_of_weight += this->distribution(i + 1);
        score[i] = get_score(layer[i]);
    }
    int rng = rand() % sum_of_weight;
    int rank = -1;
    for (int i = 0; i < layer.get_size(); ++i) {
        rng -= this->distribution(i + 1);
        if (rng <= 0) {
            rank = i;
            break;
        }
    }
    assert(rank != -1);
    // Find the node index that has the correct rank
    std::sort(score, score + layer.get_size(), [&](int a, int b) { return b < a; });
    for (int i = 0; i < layer.get_size(); ++i) {
        if (score[rank] == get_score(layer[i])) return i;
    }

    // int64_t min_score = (int64_t)get_score(layer[0]);
    // int64_t total_score = min_score;
    // for (int i = 1; i < layer.get_size(); ++i) {
    //     int64_t node_i_score = (int64_t)get_score(layer[i]);
    //     total_score += node_i_score;
    //     if (min_score > node_i_score) min_score = node_i_score;
    // }
    // total_score -= min_score * (int64_t)layer.get_size();
    // int64_t rng = (int64_t)(((double)rand() / ((double)RAND_MAX + 1.0)) * ((double)total_score + 1.0));
    // for (int i = 0; i < layer.get_size(); ++i) {
    //     rng -= (int64_t)get_score(layer[i]) - min_score;
    //     if (rng <= 0) {
    //         return i;
    //     }
    // }

    assert(false);
    return -1;
};

// Make a node attempt a move
void Tree::attempt(NodeState& node_state, PieceData& placement)
{
    // Check soft drop
    node_state.lock.softdrop = !node_state.state.board.is_above_stack(placement);

	// Update queue
    assert(node_state.state.queue.get_size() > 0);
    bool used_hold = (placement.type != node_state.state.queue[0]);
    if (used_hold) {
        if (node_state.state.hold == PIECE_NONE) {
            assert(node_state.state.queue.get_size() > 1);
            node_state.state.hold = node_state.state.queue[0];
            node_state.state.queue.erase(0);
        }
        else {
            PieceType placeholder = node_state.state.hold;
            node_state.state.hold = node_state.state.queue[0];
            node_state.state.queue[0] = placeholder;
        }
    }
    node_state.state.queue.erase(0);

	// Check t spin, place piece and clear line
	bool is_t_spin = node_state.state.board.is_t_spin(placement);
	node_state.state.board.place_piece(placement);
	int line_cleared = node_state.state.board.clear_line();

	// Update ren, b2b and lock data
	if (line_cleared > 0) {
		++node_state.state.ren;
		// T spin
		if (is_t_spin) {
			++node_state.state.b2b;
			node_state.lock.type = (LockType)((int)LOCK_TSPIN_1 + (line_cleared - 1));
		}
		else {
			// Perfect clear
			if (node_state.state.board.column[0] == (uint64_t)0 &&
				node_state.state.board.column[1] == (uint64_t)0 &&
				node_state.state.board.column[2] == (uint64_t)0 &&
				node_state.state.board.column[3] == (uint64_t)0 &&
				node_state.state.board.column[4] == (uint64_t)0 &&
				node_state.state.board.column[5] == (uint64_t)0 &&
				node_state.state.board.column[6] == (uint64_t)0 &&
				node_state.state.board.column[7] == (uint64_t)0 &&
				node_state.state.board.column[8] == (uint64_t)0 &&
				node_state.state.board.column[9] == (uint64_t)0) {
				node_state.lock.type = LOCK_PC;
			}
			else {
				// Normal clear line
				if (line_cleared == 4) {
					++node_state.state.b2b;
                }
				else {
					node_state.state.b2b = 0;
                }
				node_state.lock.type = (LockType)((int)LOCK_CLEAR_1 + (line_cleared - 1));
			}
		}
	}
	else {
		node_state.state.ren = 0;
		node_state.lock.type = LOCK_NONE;
	}
};

// Expand a node to a layer
void Tree::expand(vec<Node>& layer, NodeState& node_state)
{
    // Sanity check
    if (node_state.state.queue.get_size() == 0) {
        return;
    }

    // For current piece
    PieceData current[MAX_MOVE_GENERATOR];
    int current_count = 0;
    MoveGenerator::generate(node_state.state.board, node_state.state.queue[0], current, current_count);
    for (int i = 0; i < current_count; ++i) {
        layer.add({ .parent = nullptr, .children = vec<Node>(false), .placement = current[i], .score = Score() });
    }

    // For hold piece
    if (node_state.state.hold != node_state.state.queue[0]) {
        // Find the piece to expand
        PieceType hold_piece = node_state.state.hold;
        if (hold_piece == PIECE_NONE && node_state.state.queue.get_size() > 1) {
            hold_piece = node_state.state.queue[1];
        }
        if (hold_piece == PIECE_NONE || hold_piece == node_state.state.queue[0]) return;

        // Generate
        PieceData hold[MAX_MOVE_GENERATOR];
        int hold_count = 0;
        MoveGenerator::generate(node_state.state.board, hold_piece, hold, hold_count);
        for (int i = 0; i < hold_count; ++i) {
            layer.add({ .parent = nullptr, .children = vec<Node>(false), .placement = hold[i], .score = Score() });
        }
    }
};

// Expand a node
// Evaluate the children
void Tree::expand(Node* node, NodeState& node_state)
{
    // Borrow a chunk from the pool
    bool borrow_success = this->pool.get_chunk(node->children);
    if (!borrow_success) return;

    // Expand and fill in the chunk
    this->expand(node->children, node_state);

    // If there is no children, then return the chunk
    if (node->children.get_size() == 0) {
        this->pool.return_chunk(node->children);
        return;
    }

    // Evaluate and set parent
    for (int i = 0; i < node->children.get_size(); ++i) {
        // Set the parent pointer
        node->children[i].parent = node;

        // Evaluate children
        NodeState child_state = node_state;
        this->attempt(child_state, node->children[i].placement);
        this->eval.evaluate(node->children[i], node_state);
    }

    // Update node count
    this->node_count += node->children.get_size();
};

// Release a tree branch
// We don't free memory, we only return the borrowed chunk to the pool
void Tree::release(Node* node)
{
    // Sanity check
    if (node->children.iter_begin() == nullptr) {
        return;
    }

    // Release every child
    for (int i = 0; i < node->children.get_size(); ++i) {
        // this->release(&node->children[i]);
        this->release(node->children.iter_begin() + i);
    }

    // Update node count
    this->node_count -= node->children.get_size();

    // Return the borrowed chunk to the pool
    this->pool.return_chunk(node->children);
};

// Backpropagate from a leaf node
void Tree::backpropagate(Node* node)
{
    // If node has children, then we should update the score
    if (node->children.get_size() > 0) {
        // Get the best child
        int max_child_score = get_score(node->children[0]);
        for (int i = 1; i < node->children.get_size(); ++i) {
            if (max_child_score < get_score(node->children[i])) {
                max_child_score = get_score(node->children[i]);
            }
        }

        // If there is a new best child, update score
        if (max_child_score > node->score.best) {
            node->score.best = max_child_score;
            if (node->parent != nullptr) this->backpropagate(node->parent);
        }
    }
    // Else, just go up
    else {
        if (node->parent != nullptr) this->backpropagate(node->parent);
    }
};

// One iteration of the tree search
void Tree::think()
{
    // Sanity check
    if (this->candidate.get_size() == 0) return;

    // Traverse down the tree to a leaf node
    int leaf_depth = 1;
    int candidate_index = this->select(this->candidate);
    Node* leaf = this->candidate.iter_begin() + candidate_index;
    NodeState leaf_state = this->root;
    this->attempt(leaf_state, this->candidate[candidate_index].placement);
    while (true)
    {
        // Select the next child to traverse
        int next_index = this->select(leaf->children);

        // If the next child's index is -1 then we found a leaf
        if (next_index == -1) break;

        // Else the next index is valid, then traverse
        this->attempt(leaf_state, leaf->children[next_index].placement);
        leaf = leaf->children.iter_begin() + next_index;

        // Update leaf's depth
        ++leaf_depth;
    }
    
    // Expand the leaf node
    this->expand(leaf, leaf_state);

    // Backpopagate from the leaf
    this->backpropagate(leaf);

    // Update depth
    if (leaf->children.get_size() > 0) ++leaf_depth;
    this->depth = std::max(this->depth, leaf_depth);
};

// Tree search for a certain number of iteration
void Tree::search(int iteration)
{
    for (int i = 0; i < iteration; ++i) {
        this->think();
    }
};

// Pick the best move with incomming garbage taken into account
bool Tree::pick(PieceData& result, int incomming_attack)
{
    // Reset the result
    result = PieceData();

    // Find current board height in the center
    int root_height[10];
    this->root.state.board.get_height(root_height);
    int max_height_center = *std::max_element(root_height + 3, root_height + 7);

    // Find the best move
    // The best move is the move that has the highest score and is able to survive the incomming attack
    Node best_node;
    for (int i = 0; i < this->candidate.get_size(); ++i) {
        // Get the node state of the move
        NodeState node_state = this->root;
        this->attempt(node_state, this->candidate[i].placement);

        // If the move can survive incomming attack
        if (max_height_center + incomming_attack - Evaluator::spike(this->root, this->candidate[i], node_state) <= 20) {
            if (best_node.placement.type == PIECE_NONE) {
                best_node = this->candidate[i];
            }
            else {
                if (best_node < this->candidate[i]) {
                    best_node = this->candidate[i];
                }
            }
        }
    }

    // Check if tree is death
    if (best_node.placement.type == PIECE_NONE) {
        return false;
    }

    // Set the result
    result = best_node.placement;

    // Node best = this->candidate[0];
    // for (int i = 1; i < this->candidate.get_size(); ++i) {
    //     if (best < this->candidate[i]) best = this->candidate[i];
    // }

    // result = best.placement;

    return true;
};

// Get the tree's statistics
void Tree::stat(int& node, int& depth) 
{
    node = this->node_count;
    depth = this->depth;
};