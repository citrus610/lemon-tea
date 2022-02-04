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
bool TranspositionTable::add(uint32_t hash, int attack)
{
    TranspositionEntry* slot = this->bucket[hash & (this->size - 1)].slot;

    // If there is matching hash key, replace if new attack is better
    for (int i = 0; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == hash) {
            if (slot[i].attack < attack) {
                slot[i].attack = attack;
                return true;
            }
            return false;
        }
    }

    // Else, if there is an empty slot in the bucket, just fill in
    for (int i = 0; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == 0 && slot[i].attack == 0) {
            slot[i].key = hash;
            slot[i].attack = attack;
            return true;
        }
    }

    // Else, try to replace the slot with smallest attack eval
    int smallest_attack_index = 0;
    for (int i = 1; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].attack < slot[smallest_attack_index].attack) {
            smallest_attack_index = i;
        }
    }
    if (slot[smallest_attack_index].attack < attack) {
        slot[smallest_attack_index].key = hash;
        slot[smallest_attack_index].attack = attack;
        return true;
    }

    return false;
};

// Get the Transposition entry
// Return true if found matching key
bool TranspositionTable::get(uint32_t hash, int& attack)
{
    TranspositionEntry* slot = this->bucket[hash & (this->size - 1)].slot;
    for (int i = 0; i < TRANSPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == hash) {
            attack = slot[i].attack;
            return true;
        }
    }
    return false;
};

// Take a sample of the first 1000 buckets in the table
// Estimate the permill of used buckets in hash table
int TranspositionTable::hashfull()
{
    int used = 0;
    for (uint32_t i = 0; i < std::min(uint32_t(1000), this->size); ++i) {
        for (int k = 0; k < TRANSPOSITION_BUCKET_SIZE; ++k) {
            if (this->bucket[i].slot[k].key != 0) {
                used += 1;
            }
        }
    }
    return used / TRANSPOSITION_BUCKET_SIZE;
};

};