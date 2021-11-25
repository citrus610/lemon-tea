#pragma once

#include "eval.h"
#include "genmove.h"

constexpr int MAX_TREE_QUEUE = 16;
constexpr int MAX_TREE_LAYER_SIZE = 25600;
constexpr int MAX_TREE_LAYER_FORECAST_SIZE = 400;
constexpr int MAX_TREE_LAYER_FORECAST_DEPTH = 8;
constexpr int MAX_TREE_CANDIDATE = 128;

struct Best {
	Node node;
	NodeForecast forecast;
	Node spike;
	int spike_count = 0;
};

class Tree
{
public:
	PieceType queue[MAX_TREE_QUEUE]; int queue_count = 0;
	PieceType bag[7]; int bag_count = 0;
	Node root;
	Best best;
	vec<Node> layer[MAX_TREE_QUEUE + 2];
	vec<NodeForecast> layer_forecast[MAX_TREE_LAYER_FORECAST_DEPTH];
public:
	int beam = 200; // should be const
	bool forecast = false;
public:
	Evaluator evaluator;
public:
	void init(bool forecast);
	void clear();
	void clear_forecast();
	bool set(BitBoard board, PieceType current, PieceType hold, PieceType next[MAX_TREE_QUEUE], int next_count, int b2b, int ren, bool init);
	bool advance(Action& action, PieceType new_piece[MAX_TREE_QUEUE], int new_piece_count);
public:
	int search(int iteration);
	void search_one_iter(int& iter_num, int& layer_index, int& node_count);
	Node get_best(int incomming_attack);
public:
	static Node pick_from_best(Node& root, Best& candidate, int incomming_attack);
public:
	void attempt_node(Node& parent, Node& child, PieceData& placement, bool hold);
	void convert_node_forecast(Node& parent, int& node_count);
	void forecast_node(NodeForecast& node_forecast, int& node_count);
public:
	void push_node_forecast(NodeForecast& node, vec<NodeForecast>& new_layer);
	void expand_node(Node& parent, vec<Node>& new_layer, int& node_count);
	void expand_node_forecast(NodeForecast& parent, vec<NodeForecast>& new_layer, int& node_count);
	void expand_layer(vec<Node>& previous_layer, vec<Node>& new_layer, int& width, int& node_count);
	void expand_layer_forecast(vec<NodeForecast>& previous_layer, vec<NodeForecast>& new_layer, int& width, int& node_count);
};

