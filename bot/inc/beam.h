#ifndef BEAM_H
#define BEAM_H

#include "genmove.h"
#include "pathfinder.h"
#include "eval.h"

class beam
{
public:
	static const int max_depth = 16;
	static const int max_node_per_layer = 16000;
	static const int max_reward = 256;
	static const int prune = 2000;
	static const int prune_percentage = 20;
	const int width_s = 50;
	const int width_l = 150;
	bool is_forecast = true;
	int forecast_bag_index = 0;
	int preview = 0;
	int depth = -1;

	piece_type queue[max_depth]; int queue_count = 0;
	reward reward[max_reward]; int reward_count = 0;
	vec<node> layer[max_depth];

	node root;
	eval evaluator;

public:
	void init(int preview, bool is_forecast);
	void reset();
	void forecast();
	void set_root(bitboard& board, piece_type current, piece_type hold, piece_type next[max_depth], int b2b, int ren);

	void expand_node(node& parent, vec<node>& new_layer, const bool& is_first, const bool& is_last, int& node_count);
	void expand(vec<node>& pre_layer, vec<node>& new_layer, const int& beam_width, const bool& is_last, int& node_count);
	int search(int iteration);
	void search_one_iter(int& iter_num, int& layer_index, int& node_count);
	int get_best_index();
};

#endif // BEAM_H