#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;

#include "battle.h"

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
	perfect_clear,
	waste_time,
	waste_T,
	waste_I,
	b2b,
	ren
);

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
	structure[2],
	structure[3],
	b2b
);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Weight, defence, attack);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SaveData, base, v1, v2, next, win_v1, win_v2, total);