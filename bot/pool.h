#pragma once

#include "node.h"

constexpr int CHUNK_SIZE = 128;

class Pool
{
private:
    vec<vec<Node>> chunk;
public:
    Pool();
    ~Pool();
public:
    void init(int init_count);
    void destroy();
public:
    bool get_chunk(vec<Node>& getter);
    void return_chunk(vec<Node>& chunk);
};
