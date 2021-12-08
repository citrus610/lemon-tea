#pragma once

#include "bitboard.h"

struct PathFinderNode 
{
	PieceData position;
	MoveType path[32] = {};
	int path_count = 0;
	int input_count = 0;

	bool operator < (PathFinderNode& other) {
		if (input_count == other.input_count) return path_count < other.path_count;
		return input_count < other.input_count;
	}

	bool operator > (PathFinderNode& other) {
		if (input_count == other.input_count) return path_count > other.path_count;
		return input_count > other.input_count;
	}
};

namespace PathFinder 
{
	bool search(BitBoard& board, PieceData& placement, MoveType list[32], int& list_count);
	void calculate_input(PathFinderNode& node);
	void expand(BitBoard& board, PathFinderNode& node, PathFinderNode result[4], int& result_count);
	void generate_on_stack(BitBoard& board, PieceType piece, std::vector<PathFinderNode>& result);
	int list_index(PathFinderNode& node, std::vector<PathFinderNode>& list);
};