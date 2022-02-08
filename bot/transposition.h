#pragma once

#include "hash.h"
#include "node.h"

namespace LemonTea
{

constexpr int TRANSPOSITION_BUCKET_SIZE = 4;

struct TranspositionEntry
{
    uint32_t key = 0;
    int32_t eval = 0;
};

struct TranspositionBucket
{
    TranspositionEntry slot[TRANSPOSITION_BUCKET_SIZE] = { 0 };
};

class TranspositionTable
{
public:
    TranspositionBucket* bucket = nullptr;
    uint32_t size = 0;
public:
    TranspositionTable();
    ~TranspositionTable();
public:
    static uint32_t hash(Board& board);
public:
    void init(int power);
    void destroy();
public:
    void clear();
    bool get(uint32_t hash, int& eval);
    bool add(uint32_t hash, int eval);
    bool addible(uint32_t hash, int eval);
    double hashfull();
};

};