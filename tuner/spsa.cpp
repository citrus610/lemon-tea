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

void Spsa::vary_weight(LemonTea::Weight& base, LemonTea::Weight& v1, LemonTea::Weight& v2)
{
    // Set init
    v1 = base;
    v2 = base;

    // Defence
    VARY_WEIGHT_PARAMETER(defence.height, 50);
    VARY_WEIGHT_PARAMETER(defence.height_10, 100);
    VARY_WEIGHT_PARAMETER(defence.height_15, 100);
    VARY_WEIGHT_PARAMETER(defence.bumpiness, 40);
    VARY_WEIGHT_PARAMETER(defence.bumpiness_s, 10);
    VARY_WEIGHT_PARAMETER(defence.flat, 40);
    VARY_WEIGHT_PARAMETER(defence.row_t, 50);
    VARY_WEIGHT_PARAMETER(defence.hole_a, 100);
    VARY_WEIGHT_PARAMETER(defence.hole_b, 100);
    VARY_WEIGHT_PARAMETER(defence.blocked, 35);
    VARY_WEIGHT_PARAMETER(defence.well, 25);
    for (int i = 0; i < 4; ++i) {
        VARY_WEIGHT_PARAMETER(defence.structure[i], 50);
    }
    VARY_WEIGHT_PARAMETER(defence.b2b, 50);

    // Attack
    for (int i = 0; i < 4; ++i) {
        VARY_WEIGHT_PARAMETER(attack.clear[i], 75);
    }
    for (int i = 0; i < 3; ++i) {
        VARY_WEIGHT_PARAMETER(attack.tspin[i], 75);
    }
    VARY_WEIGHT_PARAMETER(attack.waste_time, 25);
    VARY_WEIGHT_PARAMETER(attack.waste_T, 50);
    VARY_WEIGHT_PARAMETER(attack.b2b, 50);
    VARY_WEIGHT_PARAMETER(attack.ren, 75);
}

void Spsa::approach_value(int& base, int& v, double ap_v)
{
    double delta_raw = double(v) - double(base);
    double delta = delta_raw * ap_v;
    if (delta <= 1.0 && delta >= 0.5) delta = 1.0;
    base += int(delta);
}

void Spsa::approach_weight(LemonTea::Weight& base, LemonTea::Weight& v)
{
    // Setting apply factor
    double ap_v = 0.1;

    // Defence
    APPROACH_WEIGHT_PARAMETER(defence.height, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.height_10, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.height_15, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.bumpiness, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.bumpiness_s, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.flat, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.row_t, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.hole_a, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.hole_b, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.blocked, ap_v);
    APPROACH_WEIGHT_PARAMETER(defence.well, ap_v);
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
    APPROACH_WEIGHT_PARAMETER(attack.b2b, ap_v);
    APPROACH_WEIGHT_PARAMETER(attack.ren, ap_v);
}
