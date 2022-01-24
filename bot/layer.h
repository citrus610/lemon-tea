#pragma once

#include "node.h"

namespace LemonTea
{

constexpr int LAYER_SIZE_INIT = 16000;

class Layer
{
public:
    std::vector<Node> data;
    int taken_count = 0;
public:
    Layer();
public:
    void init();
};

};