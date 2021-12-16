#pragma once

#include "bitboard.h"

constexpr int GAME_MAX_QUEUE = 32;

struct GameState
{
	BitBoard board;
	PieceType hold = PIECE_NONE;
    arrayvec<PieceType, 32> queue = arrayvec<PieceType, 32>();
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
	int best = INT_MIN;
};

struct Node
{
	Node* parent = nullptr;
	vec<Node> children = vec<Node>(false);
	PieceData placement = PieceData();
	Score score = Score();
};

static inline int get_score(Score& score) 
{
	if (score.best == INT_MIN) {
		return score.attack + score.defence;
	}
	return score.attack + score.best;
};

static inline int get_score(Node& node) 
{
	return get_score(node.score);
};

static inline bool operator < (Score& a, Score& b) 
{
	return get_score(a) < get_score(b);
};

static inline bool operator < (Node& a, Node& b) 
{
	return a.score < b.score;
};