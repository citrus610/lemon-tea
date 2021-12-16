#pragma once

#pragma once

#include "eval.h"
#include "genmove.h"
#include "pathfinder.h"
#include "pool.h"

constexpr int TREE_MAX_DEPTH = 32;

class Tree
{
private:
    Pool pool;
    Evaluator eval;
    int node_count;
    int depth;
public:
    arrayvec<PieceType, 7> bag;
    NodeState root;
    vec<Node> candidate = vec<Node>(false);
public:
    int min_pool = 125000;
    bool forecast = false;
public:
    Tree();
    ~Tree();
public:
    void init(PieceType* init_queue, int queue_count);
    void reset(BitBoard board, int b2b, int ren);
    bool advance(PieceData placement, PieceType* new_piece, int new_piece_count);
    void destroy();
public:
    int distribution(int x);
    int select(vec<Node>& layer);
    void attempt(NodeState& node_state, PieceData& placement);
    void expand(vec<Node>& layer, NodeState& node_state);
    void expand(Node* node, NodeState& node_state);
    void release(Node* node);
    void backpropagate(Node* node);
    void think();
    void search(int iteration);
public:
    void stat(int& node, int& depth);
    bool pick(PieceData& result, int incomming_attack);
};
