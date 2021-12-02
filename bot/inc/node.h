#pragma once

#include "bitboard.h"

constexpr int MAX_NODE_ACTION = 20;

struct GameState
{
	BitBoard board;
	PieceType current = PIECE_NONE;
	PieceType hold = PIECE_NONE;
	int next = 0;
	int b2b = 0;
	int ren = 0;
};

struct Action
{
	PieceData placement = {
		-1,
		-1,
		PIECE_NONE,
		PIECE_UP
	};
	bool hold = false;
	bool soft_drop = false;
	LockData lock = LOCK_NONE;
};

struct Score
{
	int attack = 0;
	int defence = 0;
};

struct Node
{
	GameState state;
	Action origin;
	Action action;
	Score score;

	bool operator < (Node& other) {
		return score.attack + score.defence < other.score.attack + other.score.defence;
	};
};

struct NodeForecast {
	Node parent;
	Node children[7];
	PieceType bag[7];
	int bag_count = 0;
	int score = 0;

	bool operator < (NodeForecast& other) {
		return score < other.score;
	};
};