#pragma once

#include "../bot/bot.h"
#include <iomanip>
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;

namespace LemonTea
{

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
    ren,
    ren_10,
    ren_15
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WeightDefence,
    height,
    height_10,
    height_15,
    bumpiness,
    bumpiness_s,
    flat,
    row_t,
    hole_a,
    hole_b,
    blocked,
    well,
    sidewell,
    structure[0],
    structure[1],
    structure[2],
    structure[3],
    b2b
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Weight, defence, attack)

};