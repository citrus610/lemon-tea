#pragma once

#include "board.h"

namespace LemonTea
{

constexpr int MOVE_GENERATION_POSITION_MAX = 128;

class PositionMap
{
public:
    uint32_t data[10][4] = { 0 };
public:
    void push(const Piece& piece);
    bool exist(const Piece& piece);
};

class MoveGenerator
{
public:
    static void expand(Board& board, Piece& piece, Piece result[4], int& result_count);
    static void generate(Board& board, PieceType type, Piece result[MOVE_GENERATION_POSITION_MAX], int& result_count);
};

};