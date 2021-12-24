#pragma once

#include "../bot/bot.h"
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WeightAttack,
	clear[0],
	clear[1],
	clear[2],
	clear[3],
	tspin[0],
	tspin[1],
	tspin[2],
	perfect_clear,
	waste_time,
	waste_T,
	waste_I,
	b2b,
	ren
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WeightDefence,
	max_height,
	max_height_top_half,
	max_height_top_quarter,
	bumpiness,
	bumpiness_s,
	flat,
	transition_row,
	transition_column,
	hole,
	hole_s,
	crack,
	crack_s,
	blocked,
	blocked_s,
	well[0],
	well[1],
	well[2],
	well[3],
	well[4],
	well[5],
	well[6],
	well[7],
	well[8],
	well[9],
	structure[0],
	structure[1],
	b2b
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Weight, defence, attack)