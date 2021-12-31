#pragma once

#include "eval.h"
#include "movegen.h"
#include "layer.h"

constexpr int MAX_TREE_QUEUE = 16;
constexpr int MAX_TREE_LAYER_SIZE = 25600;
constexpr int MAX_TREE_CANDIDATE = 128;
constexpr int INIT_TREE_LAYER_GROUP[4] = { 1, 4, 16, 32 };

struct Best {
	Node node;
	Node spike;
	int spike_count = 0;
};

class Tree
{
public:
	arrayvec<PieceType, MAX_TREE_QUEUE> queue;
    arrayvec<PieceType, 7> bag;
	Node root;
	Best best;
	Layer layer[MAX_TREE_QUEUE];
public:
	int beam = 50;
public:
	Evaluator evaluator;
public:
	void init(PieceType* init_queue, int queue_count);
    void reset(BitBoard board, int b2b, int ren);
    bool advance(Action action, PieceType* new_piece, int new_piece_count);
    void clear();
public:
	int search(int iteration);
	void think(int& iter_num, int& layer_index, int& node_count);
	Node solution(int incomming_attack);
public:
	static Node pick_from_best(Node& root, Best& candidate, int incomming_attack);
public:
	void attempt_node(Node& parent, Node& child, PieceData& placement, bool hold);
	void expand_node(Node& parent, Layer& new_layer, int& node_count);
	void expand_layer(Layer& previous_layer, Layer& new_layer, int& width, int& node_count);
};

