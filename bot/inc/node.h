#ifndef NODE_H
#define NODE_H

#include "bitboard.h"

struct node {

	// Game state
	bitboard board;
	piece_type current = PIECE_NONE;
	piece_type hold = PIECE_NONE;
	int next = 0;
	int b2b = 0;
	int ren = 0;

	// Lock state
	bool is_soft_srop = false;
	bool is_hold = false;
	lock_data lock = LOCK_NONE;

	// Eval state
	int clear[4] = { 0, 0, 0, 0 };
	int tspin[3] = { 0, 0, 0 };
	int pc = 0;
	int max_b2b = 0;
	int max_ren = 0;
	int waste_T = 0;
	int waste_I = 0;
	int waste_time = 0;
	int structure[2] = { 0, 0 };
	int pre_structure[2] = { 0, 0 };
	int waste_structure[2] = { 0, 0 };

	// Node state
	int score = -999999999;
	int org_index = -1;
	bool operator < (const node& other) { return score < other.score; };
	void attemp(node& parent, piece_data& piece, bool is_hold, piece_type queue[16], int& queue_count);
};

/*
* Instead of choosing node that lead to the best possible outcome, we choose node which has more offsprings
*/
struct reward {
	int visit = 0;
	int score = 0;
	bitboard board;
	piece_data placement;

	bool operator < (const reward& other) {
		if (visit == other.visit)
			return score < other.score;
		else
			return visit < other.visit;
		//if (visit == 0) return true;
		//if (other.visit == 0) return false;
		//return (score / visit) < (other.score / other.visit);
	}
	bool operator > (const reward& other) {
		if (visit == other.visit)
			return score > other.score;
		else
			return visit > other.visit;
		//if (visit == 0) return false;
		//if (other.visit == 0) return true;
		//return (score / visit) > (other.score / other.visit);
	}
};

#endif // NODE_H