#include "layer.h"

namespace LemonTea
{

Layer::Layer()
{
    this->taken_count = 0;
};

void Layer::init()
{
    this->taken_count = 0;
    this->data.reserve(LAYER_SIZE_INIT);
    this->data.clear();
};

};