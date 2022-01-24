#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;

#include "battle.h"

namespace LemonTea
{

struct SaveData
{
    Weight base;
    Weight v1;
    Weight v2;
    Weight next;

    int win_v1 = 0;
    int win_v2 = 0;
    int total = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WeightAttack,
    clear[0],
    clear[1],
    clear[2],
    clear[3],
    tspin[0],
    tspin[1],
    tspin[2],
    pc,
    waste_time,
    waste_T,
    b2b,
    ren
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WeightDefence,
    height,
    height_10,
    height_15,
    bumpiness,
    bumpiness_s,
    flat,
    row_t,
    column_t,
    hole,
    crack,
    blocked,
    well,
    structure[0],
    structure[1],
    structure[2],
    structure[3],
    b2b
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Weight, defence, attack)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SaveData, base, v1, v2, next, win_v1, win_v2, total);

};