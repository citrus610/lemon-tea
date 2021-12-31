#pragma once

#include "bitboard.h"

struct GameState
{
	BitBoard board = BitBoard();
	PieceType current = PIECE_NONE;
	PieceType hold = PIECE_NONE;
	int next = 0;
	int b2b = 0;
	int ren = 0;
};

struct Action
{
	PieceData placement = PieceData();
	LockType lock = LOCK_NONE;
    bool hold = false;
	bool soft_drop = false;
};

struct Score
{
	int attack = 0;
	int defence = 0;
};

struct Node
{
	GameState state = GameState();
	Action action = Action();
	Action origin = Action();
	Score score = Score();
};

static bool operator < (Score& a, Score& b) {
    return a.attack + a.defence < b.attack + b.defence;
}

static bool operator < (Node& a, Node& b) {
    return a.score < b.score;
}