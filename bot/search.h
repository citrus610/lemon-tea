#pragma once

#include "eval.h"
#include "movegen.h"
#include "layer.h"

namespace LemonTea
{

constexpr int SEARCH_QUEUE_MAX = 16;
constexpr int SEARCH_PRUNE = 8;

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
    Evaluator evaluator;
    int beam = 200;
public:
    void init(PieceType* init_queue, int queue_count);
    void reset(Board board, int b2b, int ren);
    bool advance(NodeAction action, PieceType* new_piece, int new_piece_count);
    void clear();
public:
    void expand_node(Node& parent, Layer& new_layer, int& node_count);
    void expand_layer(Layer& previous_layer, Layer& new_layer, int& width, int& node_count);
    void search(int iteration, int& node, int& depth);
    void think(int& iter_num, int& layer_index, int& node_count);
    Node solution(int incomming_attack);
public:
    static Node pick_from_best(Node& root, SearchBest& candidate, int incomming_attack);
};

};