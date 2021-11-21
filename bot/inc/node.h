#pragma once

#include "bitboard.h"

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
	Action action;
	Action origin;
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
};