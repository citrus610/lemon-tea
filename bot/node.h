#pragma once

#include "board.h"

namespace LemonTea
{

struct NodeState
{
    Board board = Board();
    PieceType current = PIECE_NONE;
    PieceType hold = PIECE_NONE;
    int next = 0;
    int b2b = 0;
    int ren = 0;
};

struct NodeAction
{
    Piece placement = Piece();
    LockType lock = LOCK_NONE;
    bool hold = false;
    bool softdrop = false;
};

struct NodeScore
{
    int attack = 0;
    int defence = 0;
};

class Node
{
public:
    NodeState state = NodeState();
    NodeAction action = NodeAction();
    NodeAction origin = NodeAction();
    NodeScore score = NodeScore();
public:
    void attempt(Piece& placement, bool hold, PieceType* queue, int queue_size);
};

struct NodeForecast
{
    Node parent = Node();
    arrayvec<Node, 7> children = arrayvec<Node, 7>();
    arrayvec<PieceType, 7> bag = arrayvec<PieceType, 7>();
};

static bool operator < (NodeScore& a, NodeScore& b) {
    return a.attack + a.defence < b.attack + b.defence;
}

static bool operator < (Node& a, Node& b) {
    return a.score < b.score;
}

static bool operator < (NodeForecast& a, NodeForecast& b) {
    return a.parent < b.parent;
}

};