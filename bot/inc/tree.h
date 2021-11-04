#pragma once

#include "eval.h"
#include "genmove.h"

constexpr int MAX_TREE_QUEUE = 16;
constexpr int MAX_TREE_LAYER_SIZE = 25600;

class Tree
{
public:
	PieceType queue[MAX_TREE_QUEUE]; int queue_count = 0;
	Node root;
	Node best;
	vec<Node> layer[MAX_TREE_QUEUE + 2];
public:
	int beam = 200;
	bool forecast = false;
public:
	Evaluator evaluator;
public:
	void init();
	void clear();
	void set(BitBoard board, PieceType current, PieceType hold, PieceType next[MAX_TREE_QUEUE], int next_count, int b2b, int ren);
	bool advance(Action& action, PieceType new_piece[MAX_TREE_QUEUE], int new_piece_count);
	void attempt_node(Node& parent, Node& child, PieceData& placement, bool hold);
	void expand_node(Node& parent, vec<Node>& new_layer, int& node_count);
	void expand_layer(vec<Node>& previous_layer, vec<Node>& new_layer, int& width, int& node_count);
public:
	int search(int iteration);
	void search_one_iter(int& iter_num, int& layer_index, int& node_count);
	Node get_best();
};

