#pragma once

#include "bitboard.h"

constexpr int MAX_MOVE_GENERATOR = 128;

namespace MoveGenerator
{
    void generate(BitBoard& board, PieceType piece, PieceData result[MAX_MOVE_GENERATOR], int& result_count);
	int is_in(PieceData& placement, PieceData list[MAX_MOVE_GENERATOR], int& list_count, bool exact);
	void expand(BitBoard& board, PieceData& piece, PieceData result[4], int& result_count);
};