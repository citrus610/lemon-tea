#include "pool.h"

Pool::Pool()
{
}

Pool::~Pool()
{
    this->destroy();
}

void Pool::init(int init_count)
{
    this->chunk.init(init_count);
    for (int i = 0; i < init_count; ++i) {
        vec<Node> new_chunk;
        new_chunk.init(CHUNK_SIZE);
        this->chunk.add(new_chunk);
    }
}

void Pool::destroy()
{
    assert(this->chunk.get_size() == this->chunk.get_capacity());
    for (int i = 0; i < this->chunk.get_size(); ++i) {
        this->chunk[i].destroy();
    }
    this->chunk.destroy();
}

bool Pool::get_chunk(vec<Node>& getter)
{
    if (this->chunk.get_size() == 0) {
        return false;
    }
    getter = this->chunk[this->chunk.get_size() - 1];
    this->chunk[this->chunk.get_size() - 1] = vec<Node>();
    this->chunk.pop();
    getter.clear();
    return true;
}

void Pool::return_chunk(vec<Node>& chunk)
{
    chunk.clear();
    this->chunk.add(chunk);
    chunk = vec<Node>();
}