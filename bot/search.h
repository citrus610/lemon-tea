#pragma once

#include "eval.h"
#include "movegen.h"
#include "layer.h"
#include "transposition.h"

namespace LemonTea
{

constexpr int SEARCH_QUEUE_MAX = 12;
constexpr int SEARCH_PRUNE = 8;
constexpr int SEARCH_TABLE_POWER_SIZE = 5;
constexpr int SEARCH_TABLE_POWER[SEARCH_QUEUE_MAX] = { 0, 8, 9, 10, 11 };
constexpr int SEARCH_FORECAST_TABLE_POWER = 12;
constexpr int SEARCH_FORECAST_PERCENT = 50;
constexpr int SEARCH_FORECAST_BOUND = 50;

struct SearchBest {
    Node node;
    Node spike;
    int spike_count = 0;
};

struct SearchState
{
    arrayvec<PieceType, SEARCH_QUEUE_MAX> queue = arrayvec<PieceType, SEARCH_QUEUE_MAX>();
    arrayvec<PieceType, 7> bag = arrayvec<PieceType, 7>();
    Node root = Node();
};

class Search
{
public:
    SearchState state;
    SearchBest best;
    Layer layer[SEARCH_QUEUE_MAX];
    TranspositionTable table[SEARCH_QUEUE_MAX];
    TranspositionTable ftable;
    std::vector<Node> fsearch_bound;
    Evaluator evaluator;
    int beam = 200;
public:
    void init(PieceType* init_queue, int queue_count);
    void reset(Board board, int b2b, int ren);
    bool advance(NodeAction action, PieceType* new_piece, int new_piece_count);
    void clear();
public:
    void expand_node(Node& parent, Layer& new_layer, int table_index, int& node_count);
    void expand_layer(Layer& previous_layer, Layer& new_layer, int table_index, int& node_count);
public:
    void forecast(NodeForecast& fnode, int& node_count);
    bool fsearch_qualify(Node& node);
    void fsearch(Node& node, arrayvec<PieceType, 7>& fbag, int fdepth, int& node_count);
public:
    void search(int iteration, int& node, int& depth);
    void think(int& iter_num, int& layer_index, int& node_count);
    Node solution(int incomming_attack);
public:
    static Node pick_from_best(Node& root, SearchBest& candidate, int incomming_attack);
};

};