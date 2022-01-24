#pragma once

#include "debug.h"

namespace LemonTea
{

struct BotCandidate 
{
    SearchBest best;
    Node root;
    int node = 0;
    int depth = 0;
};

struct BotSolution 
{
    Board original_board;
    NodeAction action;
    MoveType move[32];
    int move_count = 0;
    int node = 0;
    int depth = 0;
};

struct BotState 
{
    Board board;
    PieceType current = PIECE_NONE;
    PieceType hold = PIECE_NONE;
    PieceType queue[SEARCH_QUEUE_MAX];
    int queue_count = 0;
    int b2b = 0;
    int ren = 0;
};

struct BotAction 
{
    NodeAction action;
    PieceType new_piece[SEARCH_QUEUE_MAX];
    int new_piece_count = 0;
};

struct BotSetting 
{
    Weight weight = DEFAULT_WEIGHT();
    bool forecast = false;
};

class Bot
{
    std::mutex mutex;
    std::vector<BotState> state_buffer;
    std::vector<BotAction> action_buffer;
    std::vector<BotCandidate> candidate_buffer;
    bool running = false;
public:
    std::thread* thread = nullptr;
public:
    void init_thread(BotSetting setting, PieceType queue[SEARCH_QUEUE_MAX], int queue_count);
    void end_thread();
    void reset_state(Board board, int b2b, int ren);
    void advance_state(BotAction action);
    bool request_solution(BotSolution& solution, int incomming_attack);
    bool is_running();
};

};