#include "pool.h"

Pool::Pool()
{
    this->chunk = vec<vec<Node>>(false);
}

Pool::~Pool()
{
    this->destroy();
}

void Pool::init(int init_count)
{
    this->chunk.init(init_count);
    for (int i = 0; i < init_count; ++i) {
        vec<Node> new_chunk(false);
        new_chunk.init(CHUNK_SIZE);
        this->chunk.add(new_chunk);

        // THE ABOVE CODE IS BAD
        // BECAUSE  WHEN WE CREATE new_chunk AND init() IT
        // AT THE END OF THE SCOPE, THE POINTER WILL BE FREE
        // this->chunk.add(vec<Node>(false));
        // this->chunk[i].init(CHUNK_SIZE);
        // IF WE CREATE new_chunk LIKE ABOVE, then its POINTER WILL BE FREED HERE, AND THAT IS BAD!
        // WE SHOULD MAKE A CHUNK STRUCT THAT DON'T AUTOMATICALLY DESTRUCT ITSELF WHEN OUT OF SCOPE
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
    this->chunk[this->chunk.get_size() - 1] = vec<Node>(false);
    this->chunk.pop();
    getter.clear();
    return true;
}

void Pool::return_chunk(vec<Node>& chunk)
{
    chunk.clear();
    this->chunk.add(chunk);
    chunk = vec<Node>(false);
}