#include "tranposition.h"

namespace LemonTea
{

TranpositionTable::TranpositionTable()
{
    this->size = 0;
    this->bucket = nullptr;
};

TranpositionTable::~TranpositionTable()
{
    if (this->bucket != nullptr) {
        this->destroy();
    }
};

uint32_t TranpositionTable::hash(Board& board)
{
    return murmur_hash_3_x86_32((const uint8_t*)board.data, 80, 0b1010101010101010);
};

void TranpositionTable::init(int power)
{

    this->size = 1 << power;
    this->bucket = (TranpositionBucket*)malloc(this->size * sizeof(TranpositionBucket));
    this->clear();
};

void TranpositionTable::destroy()
{
    if (this->bucket == nullptr) {
        return;
    }
    free(this->bucket);
    this->bucket = nullptr;
};

void TranpositionTable::clear()
{
    if (this->bucket == nullptr) {
        return;
    }
    memset(this->bucket, 0, sizeof(TranpositionBucket) * this->size);
};

// Add an entry to the table
// Return true if add successfully
bool TranpositionTable::add(uint32_t hash, int attack)
{
    TranpositionEntry* slot = this->bucket[hash & (this->size - 1)].slot;

    // If there is matching hash key, replace if new attack is better
    for (int i = 0; i < TRANPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == hash) {
            if (slot[i].attack < attack) {
                slot[i].attack = attack;
                return true;
            }
            return false;
        }
    }

    // Else, if there is an empty slot in the bucket, just fill in
    for (int i = 0; i < TRANPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == 0 && slot[i].attack == 0) {
            slot[i].key = hash;
            slot[i].attack = attack;
            return true;
        }
    }

    // Else, try to replace the slot with smallest attack eval
    int smallest_attack_index = 0;
    for (int i = 1; i < TRANPOSITION_BUCKET_SIZE; ++i) {
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

// Get the tranposition entry
// Return true if found matching key
bool TranpositionTable::get(uint32_t hash, int& attack)
{
    TranpositionEntry* slot = this->bucket[hash & (this->size - 1)].slot;
    for (int i = 0; i < TRANPOSITION_BUCKET_SIZE; ++i) {
        if (slot[i].key == hash) {
            attack = slot[i].attack;
            return true;
        }
    }
    return false;
};

// Take a sample of the first 1000 buckets in the table
// Estimate the permill of used buckets in hash table
int TranpositionTable::hashfull()
{
    int used = 0;
    for (uint32_t i = 0; i < std::min(uint32_t(1000), this->size); ++i) {
        for (int k = 0; k < TRANPOSITION_BUCKET_SIZE; ++k) {
            if (this->bucket[i].slot[k].key != 0) {
                used += 1;
            }
        }
    }
    return used / TRANPOSITION_BUCKET_SIZE;
};

};