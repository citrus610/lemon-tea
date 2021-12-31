#include "spsa.h"

Spsa::Spsa()
{
	this->generator = std::default_random_engine((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());
}

int Spsa::random(int delta)
{
	std::normal_distribution<double> distribution(0.0, (double)delta / 2.0);

	double number = distribution(this->generator);
    return int(number);
}

void Spsa::vary_value(int& base, int& v1, int& v2, int delta)
{
	int r_value = Spsa::random(delta);
	v1 = base + r_value;
	v2 = base - r_value;
}

void Spsa::vary_weight(Weight& base, Weight& v1, Weight& v2)
{
	// Set init
	v1 = base;
	v2 = base;

	// Defence
	VARY_WEIGHT_PARAMETER(defence.max_height, 2500);
	VARY_WEIGHT_PARAMETER(defence.max_height_top_half, 5000);
	VARY_WEIGHT_PARAMETER(defence.max_height_top_quarter, 5000);
	VARY_WEIGHT_PARAMETER(defence.bumpiness, 5000);
	VARY_WEIGHT_PARAMETER(defence.bumpiness_s, 100);
	VARY_WEIGHT_PARAMETER(defence.flat, 5000);
	VARY_WEIGHT_PARAMETER(defence.transition_row, 100);
	VARY_WEIGHT_PARAMETER(defence.transition_column, 100);
	VARY_WEIGHT_PARAMETER(defence.hole, 5000);
	VARY_WEIGHT_PARAMETER(defence.hole_s, 100);
	VARY_WEIGHT_PARAMETER(defence.crack, 5000);
	VARY_WEIGHT_PARAMETER(defence.crack_s, 100);
	VARY_WEIGHT_PARAMETER(defence.blocked, 5000);
	VARY_WEIGHT_PARAMETER(defence.blocked_s, 100);
	// for (int i = 0; i < 10; ++i) {
	// 	VARY_WEIGHT_PARAMETER(defence.well[i], 2000);
	// }
	for (int i = 0; i < 4; ++i) {
		VARY_WEIGHT_PARAMETER(defence.structure[i], 5000);
	}
	VARY_WEIGHT_PARAMETER(defence.b2b, 5000);

	// Attack
	for (int i = 0; i < 4; ++i) {
		VARY_WEIGHT_PARAMETER(attack.clear[i], 5000);
	}
	for (int i = 0; i < 3; ++i) {
		VARY_WEIGHT_PARAMETER(attack.tspin[i], 5000);
	}
	VARY_WEIGHT_PARAMETER(attack.waste_time, 5000);
	VARY_WEIGHT_PARAMETER(attack.waste_T, 5000);
	VARY_WEIGHT_PARAMETER(attack.waste_I, 5000);
	VARY_WEIGHT_PARAMETER(attack.b2b, 5000);
	VARY_WEIGHT_PARAMETER(attack.ren, 5000);
}

void Spsa::approach_value(int& base, int& v, double ap_v)
{
	double delta_raw = double(v) - double(base);
	double delta = delta_raw * ap_v;
	if (delta <= 1.0 && delta >= 0.5) delta = 1.0;
	base += int(delta);
}

void Spsa::approach_weight(Weight& base, Weight& v)
{
	// Setting apply factor
	double ap_v = 0.1;

	// Defence
	APPROACH_WEIGHT_PARAMETER(defence.max_height, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.max_height_top_half, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.max_height_top_quarter, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.bumpiness, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.bumpiness_s, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.flat, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.transition_row, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.transition_column, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.hole, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.hole_s, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.crack, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.crack_s, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.blocked, ap_v);
	APPROACH_WEIGHT_PARAMETER(defence.blocked_s, ap_v);
	// for (int i = 0; i < 10; ++i) {
	// 	APPROACH_WEIGHT_PARAMETER(defence.well[i], ap_v);
	// }
	for (int i = 0; i < 4; ++i) {
		APPROACH_WEIGHT_PARAMETER(defence.structure[i], ap_v);
	}
	APPROACH_WEIGHT_PARAMETER(defence.b2b, ap_v);

	// Attack
	for (int i = 0; i < 4; ++i) {
		APPROACH_WEIGHT_PARAMETER(attack.clear[i], ap_v);
	}
	for (int i = 0; i < 3; ++i) {
		APPROACH_WEIGHT_PARAMETER(attack.tspin[i], ap_v);
	}
	APPROACH_WEIGHT_PARAMETER(attack.waste_time, ap_v);
	APPROACH_WEIGHT_PARAMETER(attack.waste_T, ap_v);
	APPROACH_WEIGHT_PARAMETER(attack.waste_I, ap_v);
	APPROACH_WEIGHT_PARAMETER(attack.b2b, ap_v);
	APPROACH_WEIGHT_PARAMETER(attack.ren, ap_v);
}
