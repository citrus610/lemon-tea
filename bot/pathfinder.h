#pragma once

#include "board.h"

namespace LemonTea
{

class PathFinderNode
{
public:
    Piece placement = Piece();
    arrayvec<MoveType, 32> path = arrayvec<MoveType, 32>();
    int input = 0;
public:
    bool operator < (PathFinderNode& other);
    bool operator > (PathFinderNode& other);
    bool operator == (PathFinderNode& other);
};

class PathFinder 
{
public:
    static bool search(Board& board, Piece& placement, MoveType list[32], int& list_count);
    static bool attempt(Board& board, PathFinderNode& parent, PathFinderNode& child, MoveType move);
    static void expand(Board& board, PathFinderNode& parent, arrayvec<PathFinderNode, 4>& children);
    static int index(PathFinderNode& node, std::vector<PathFinderNode>& vec);
    static void generate_onstack(Board& board, PieceType type, std::vector<PathFinderNode>& position);
    static void generate_understack(Board& board, std::vector<PathFinderNode>& input, std::vector<PathFinderNode>& position);
};

};