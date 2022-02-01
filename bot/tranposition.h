#pragma once

#include "hash.h"
#include "node.h"

namespace LemonTea
{

constexpr int TRANPOSITION_BUCKET_SIZE = 4;

class TranpositionEntry
{
public:
    uint32_t key = 0;
    int32_t attack = 0;
};

class TranpositionBucket
{
public:
    TranpositionEntry slot[TRANPOSITION_BUCKET_SIZE] = { 0 };
};

class TranpositionTable
{
public:
    TranpositionBucket* bucket = nullptr;
    uint32_t size = 0;
public:
    TranpositionTable();
    ~TranpositionTable();
public:
    static uint32_t hash(Board& board);
public:
    void init(int power);
    void destroy();
public:
    void clear();
    bool get(uint32_t hash, int& attack);
    bool add(uint32_t hash, int attack);
    int hashfull();
};

};