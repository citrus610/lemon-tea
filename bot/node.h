#pragma once

#include "bitboard.h"

constexpr int GAME_MAX_QUEUE = 32;

struct GameState
{
	BitBoard board;
	PieceType hold = PIECE_NONE;
    arrayvec<PieceType, 32> queue;
	int b2b = 0;
	int ren = 0;
};

struct LockState
{
	LockType type = LOCK_NONE;
	bool softdrop = false;
};


struct NodeState
{
    GameState state;
    LockState lock;
};


struct Score
{
	int attack = 0;
	int defence = 0;
	int successor = INT_MIN;
};

struct Node
{
	Node* parent;
	vec<Node> children;
	PieceData placement;
	Score score;
};

static inline int get_score(Score& score) 
{
	return score.attack + score.defence + score.successor;
};

static inline int get_score(Node& node) 
{
	return node.score.attack + node.score.defence + node.score.successor;
};

static inline bool operator < (Score& a, Score& b) 
{
	return get_score(a) < get_score(b);
};

static inline bool operator < (Node& a, Node& b) 
{
	return a.score < b.score;
};