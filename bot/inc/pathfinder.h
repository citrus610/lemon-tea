#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "bitboard.h"

struct a_star_node {
	piece_data piece;
	int score = 0;
	int rotate_count = 0;
	int drop_count = 0;
	int move_after_drop_count = 0;
	int frame_count;
	move_type path[32];
	int path_count = 0;

	bool operator < (const a_star_node& other) {
		return score > other.score;
	};

	int calculate_score(piece_data& destination);
};

class pathfinder
{
public:
	static void search(bitboard& board, piece_data& destination, move_type result[32], int& result_count);
	static int is_in_list(std::vector<a_star_node>& open_list, a_star_node& node);
};

#endif // PATHFINDER_H