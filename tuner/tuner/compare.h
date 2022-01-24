#pragma once

#include "dev.h"
#include "sprt.h"

constexpr int COMPARE_MAX_BATTLE_FRAME = 20000;
constexpr int COMPARE_MAX_BATTLE = 1000;

class Compare
{
    std::mutex mutex;
public:
    void save_json(LemonTea::SaveData& save_data, int batch_id);
    void load_json(LemonTea::SaveData& save_data, int batch_id);
public:
    LemonTea::SaveData data;
public:
    void start(LemonTea::Weight base, LemonTea::Weight w1, LemonTea::Weight w2, int total, int batch_id, int thread);
};

