#include "search.h"

namespace LemonTea
{

void Search::init(PieceType* init_queue, int queue_count)
{
    // Init setting
    this->state = SearchState();
    this->best = SearchBest();
    this->evaluator.weight = DEFAULT_WEIGHT();

    // Init memory for layer and table
    for (int i = 0; i < SEARCH_QUEUE_MAX; ++i) {
        this->layer[i] = Layer();
        this->table[i] = TranspositionTable();
        this->layer[i].init();
        this->table[i].init(SEARCH_TABLE_POWER[std::min(i, SEARCH_TABLE_POWER_SIZE - 1)]);
    }
    this->clear();

    // Set queue
    this->state.root.state.current = init_queue[0];
    for (int i = 0; i < std::min(SEARCH_QUEUE_MAX, queue_count); ++i) {
        this->state.queue.add(init_queue[i]);
    }

    // Set bag
    PieceType full_bag[7] = { PIECE_I, PIECE_J, PIECE_L, PIECE_O, PIECE_S, PIECE_T, PIECE_Z };
    this->state.bag.clear();
    int start_index = this->state.queue.get_size() - (this->state.queue.get_size() % 7);
    for (int i = 0; i < 7; ++i) {
        bool exist_in_queue = false;
        for (int queue_index = start_index; queue_index < this->state.queue.get_size(); ++queue_index) {
            if (this->state.queue[queue_index] == full_bag[i]) {
                exist_in_queue = true;
                break;
            }
        }
        if (!exist_in_queue) {
            this->state.bag.add(full_bag[i]);
        }
    }
    this->state.queue.erase(0);
};

void Search::reset(Board board, int b2b, int ren)
{
    // Reset root state
    this->state.root.state.board = board;
    this->state.root.state.b2b = b2b;
    this->state.root.state.ren = ren;

    // Reset best node
    this->best = SearchBest();

    // Clear layer & table
    this->clear();
};

bool Search::advance(NodeAction action, PieceType* new_piece, int new_piece_count)
{
    // Check if action is valid
    if (action.placement.type == PIECE_NONE ||
        this->state.root.state.board.is_colliding(action.placement.x, action.placement.y, action.placement.type, action.placement.rotation) ||
        this->state.root.state.board.get_drop_distance(action.placement) != 0) {
        return false;
    }

    // Check if new pieces are valid
    arrayvec<PieceType, 7> bag_copy = this->state.bag;
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
    this->state.root.attempt(action.placement, action.hold, this->state.queue.iter_begin(), this->state.queue.get_size());
    this->state.root.score = NodeScore();
    int queue_shift_value = this->state.root.state.next;
    this->state.root.state.next = 0;
    this->state.root.origin = NodeAction();

    // Update queue & bag
    for (int i = 0; i < queue_shift_value; ++i) {
        this->state.queue.erase(0);
    }
    for (int i = 0; i < new_piece_count; ++i) {
        this->state.queue.add(new_piece[i]);
    }
    this->state.bag = bag_copy;

    // Reset best node
    this->best = SearchBest();

    // Clear layer & table
    this->clear();

    return true;
};

void Search::clear()
{
    for (int i = 0; i < SEARCH_QUEUE_MAX; ++i) {
        this->layer[i].data.clear();
        this->layer[i].taken_count = 0;
        this->table[i].clear();
    }
};

void Search::expand_node(Node& parent, Layer& new_layer, int table_index, int& node_count)
{
    // Sanity check
    if (parent.state.current == PIECE_NONE) return;

    // Expand current piece
    Piece current_list[MOVE_GENERATION_POSITION_MAX];
    int current_list_count = 0;
    MoveGenerator::generate(parent.state.board, parent.state.current, current_list, current_list_count);
    for (int i = 0; i < current_list_count; ++i) {
        Node child = parent;
        child.attempt(current_list[i], false, this->state.queue.iter_begin(), this->state.queue.get_size());
        this->evaluator.evaluate(child, this->state.queue.iter_begin(), this->state.queue.get_size(), this->state.bag.iter_begin(), this->state.bag.get_size());
        if (parent.origin.placement.type == PIECE_NONE) child.origin = child.action;
        if (this->best.node < child) this->best.node = child;
        if (child.state.current != PIECE_NONE) {
            if (table_index == 0) {
                new_layer.data.push_back(child);
            }
            else {
                uint32_t tt_hash = TranspositionTable::hash(child.state.board);
                int32_t tt_attack = 0;
                if (this->table[table_index].get(tt_hash, tt_attack)) {
                    if (tt_attack < child.score.attack) {
                        new_layer.data.push_back(child);
                        this->table[table_index].add(tt_hash, child.score.attack);
                    }
                }
                else {
                    new_layer.data.push_back(child);
                    this->table[table_index].add(tt_hash, child.score.attack);
                }
            }
        }
    }
    node_count += current_list_count;

    // Expand hold piece
    if (parent.state.hold != parent.state.current) {
        // Find the hold piece to expand
        PieceType real_hold_piece = parent.state.hold;
        if (parent.state.hold == PIECE_NONE && parent.state.next < this->state.queue.get_size()) {
            real_hold_piece = this->state.queue[parent.state.next];
        }

        // Sanity check
        if (real_hold_piece == PIECE_NONE) return;

        Piece hold_list[MOVE_GENERATION_POSITION_MAX];
        int hold_list_count = 0;
        MoveGenerator::generate(parent.state.board, real_hold_piece, hold_list, hold_list_count);
        for (int i = 0; i < hold_list_count; ++i) {
            Node child = parent;
            child.attempt(hold_list[i], true, this->state.queue.iter_begin(), this->state.queue.get_size());
            this->evaluator.evaluate(child, this->state.queue.iter_begin(), this->state.queue.get_size(), this->state.bag.iter_begin(), this->state.bag.get_size());
            if (parent.origin.placement.type == PIECE_NONE) child.origin = child.action;
            if (this->best.node < child) this->best.node = child;
            // if (child.state.current != PIECE_NONE) new_layer.data.push_back(child);
            if (child.state.current != PIECE_NONE) {
                if (table_index == 0) {
                    new_layer.data.push_back(child);
                }
                else {
                    uint32_t tt_hash = TranspositionTable::hash(child.state.board);
                    int32_t tt_attack = 0;
                    if (this->table[table_index].get(tt_hash, tt_attack)) {
                        if (tt_attack < child.score.attack) {
                            new_layer.data.push_back(child);
                            this->table[table_index].add(tt_hash, child.score.attack);
                        }
                    }
                    else {
                        new_layer.data.push_back(child);
                        this->table[table_index].add(tt_hash, child.score.attack);
                    }
                }
            }
        }
        node_count += hold_list_count;
    }
};

void Search::expand_layer(Layer& previous_layer, Layer& new_layer, int table_index, int& node_count)
{
    int max_pop = std::min(this->beam, (int)previous_layer.data.size());

    for (int i = 0; i < max_pop; ++i) {
        // Expand the best node
        this->expand_node(previous_layer.data[0], new_layer, table_index, node_count);

        // Pop
        std::pop_heap(previous_layer.data.data(), previous_layer.data.data() + previous_layer.data.size());
        previous_layer.data.pop_back();
    }

    // Make heap next layer
    std::make_heap(new_layer.data.data(), new_layer.data.data() + new_layer.data.size());

    // Prune
    ++previous_layer.taken_count;
    if (previous_layer.taken_count >= SEARCH_PRUNE) {
        previous_layer.data.clear();
    }
    if (previous_layer.data.empty()) {
        previous_layer.taken_count = 0;
    }
};

void Search::search(int iteration, int& node, int& depth)
{
    int iter_num = 0;
    int layer_index = 0;
    int node_count = 0;

    while (iter_num < iteration) {
        this->think(iter_num, layer_index, node_count);
        ++iter_num;
    }

    node = node_count;
};

void Search::think(int& iter_num, int& layer_index, int& node_count)
{
    // Force level 1 search
    if (iter_num == 0) {
        this->expand_node(this->state.root, this->layer[0], 0, node_count);
        if (!this->layer[0].data.empty()) {
            this->best.node = this->layer[0].data[0];
            for (int i = 0; i < (int)this->layer[0].data.size(); ++i) {
                if (this->best.node < this->layer[0].data[i]) {
                    this->best.node = this->layer[0].data[i];
                }
                int node_spike = Evaluator::spike(this->state.root, this->layer[0].data[i]);
                if (node_spike > this->best.spike_count) {
                    this->best.spike = this->layer[0].data[i];
                    this->best.spike_count = node_spike;
                }
            }
        }
        layer_index = 0;
        return;
    }

    // Check if layer empty, then go up
    while (this->layer[layer_index].data.empty()) {
        if (layer_index > 0) {
            --layer_index;
        }
        else {
            return;
        }
    }

    // Expand the layer
    this->expand_layer(this->layer[layer_index], this->layer[layer_index + 1], layer_index + 1, node_count);

    // Update layer index
    ++layer_index;
    if (layer_index >= SEARCH_QUEUE_MAX - 1) {
        --layer_index;
        this->layer[SEARCH_QUEUE_MAX - 1].data.clear();
    }
};

Node Search::solution(int incomming_attack)
{
    return Search::pick_from_best(this->state.root, this->best, incomming_attack);
};

Node Search::pick_from_best(Node& root, SearchBest& candidate, int incomming_attack)
{
    // Let the result be the best node
    Node result = candidate.node;

    // If there is incomming garbage and the bot is about to die, let result be the best spike action
    int root_height[10];
    root.state.board.get_height(root_height);
    int max_height_center = *std::max_element(root_height + 3, root_height + 7);
    if (candidate.spike_count > 0 &&
        incomming_attack > 0 &&
        max_height_center + incomming_attack - Evaluator::spike(root, candidate.node) >= 20) {
        result = candidate.spike;
    }

    return result;
};

};