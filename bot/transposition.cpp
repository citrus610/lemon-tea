#include "transposition.h"

namespace LemonTea
{

TranspositionTable::TranspositionTable()
{
    this->size = 0;
    this->bucket = nullptr;
};

TranspositionTable::~TranspositionTable()
{
    if (this->bucket != nullptr) {
        this->destroy();
    }
};

uint32_t TranspositionTable::hash(Board& board)
{
    return murmur_hash_3_x86_32((const uint8_t*)board.data, 80, 0b1010101010101010);
};

void TranspositionTable::init(int power)
{

    this->size = 1 << power;
    this->bucket = (TranspositionBucket*)malloc(this->size * sizeof(TranspositionBucket));
    this->clear();
};

void TranspositionTable::destroy()
{
    if (this->bucket == nullptr) {
        return;
    }
    free(this->bucket);
    this->bucket = nullptr;
};

void TranspositionTable::clear()
{
    if (this->bucket == nullptr) {
        return;
    }
    memset(this->bucket, 0, sizeof(TranspositionBucket) * this->size);
};

// Add an entry to the table
// Return true if add successfully
bool TranspositionTable::add(uint32_t hash, int eval)
{
    TranspositionEntry* slot = this->bucket[hash & (this->size - 1)].slot;

    // If there is matching hash key, replace if new attack is better
    for (int i = 0; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == hash) {
            if (slot[i].eval < eval) {
                slot[i].eval = eval;
                return true;
            }
            return false;
        }
    }

    // Else, if there is an empty slot in the bucket, just fill in
    for (int i = 0; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == 0 && slot[i].eval == 0) {
            slot[i].key = hash;
            slot[i].eval = eval;
            return true;
        }
    }

    // Else, try to replace the slot with smallest attack eval
    int smallest_attack_index = 0;
    for (int i = 1; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].eval < slot[smallest_attack_index].eval) {
            smallest_attack_index = i;
        }
    }
    if (slot[smallest_attack_index].eval < eval) {
        slot[smallest_attack_index].key = hash;
        slot[smallest_attack_index].eval = eval;
        return true;
    }

    return false;
};

// Get the Transposition entry
// Return true if found matching key
bool TranspositionTable::get(uint32_t hash, int& eval)
{
    TranspositionEntry* slot = this->bucket[hash & (this->size - 1)].slot;
    for (int i = 0; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == hash) {
            eval = slot[i].eval;
            return true;
        }
    }
    return false;
};

// Return true if the new entry can be add
bool TranspositionTable::addible(uint32_t hash, int eval)
{
    int32_t tt_eval;
    if (this->get(hash, tt_eval)) {
        if (tt_eval < eval) {
            return true;
        }
        return false;
    }
    return true;
};

// Take a sample of the buckets in the table
// Estimate the percent of used buckets in hash table
double TranspositionTable::hashfull()
{
    int used = 0;
    for (uint32_t i = 0; i < this->size; ++i) {
        for (int k = 0; k < TRANSPOSITION_BUCKET_SIZE; ++k) {
            if (this->bucket[i].slot[k].key != 0) {
                used += 1;
            }
        }
    }
    return double(used) / double(TRANSPOSITION_BUCKET_SIZE) / double(this->size) * 100.0;
};

};