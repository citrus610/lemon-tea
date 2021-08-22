#ifndef GENMOVE_H
#define GENMOVE_H

#include "bitboard.h"

#define MAX_MOVE_GEN_COUNT 80
#define MAX_SOFT_DROP_DEPTH 2

class genmove
{
public:
	static void generate(bitboard& board, piece_type piece, piece_data result[MAX_MOVE_GEN_COUNT], int& result_count);
	static void generate_from_list(bitboard& board, piece_type piece, piece_data pre[MAX_MOVE_GEN_COUNT], int& pre_count, piece_data next[MAX_MOVE_GEN_COUNT], int& next_count);
	static int is_piece_in_list(piece_data& piece, piece_data list[MAX_MOVE_GEN_COUNT], int& list_count);
};

#endif // GENMOVE_H