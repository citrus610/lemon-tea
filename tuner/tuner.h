#pragma once

#include "battle.h"
#include "compare.h"
#include "spsa.h"

class Tuner
{
public:
    Tuner();
public:
    Battle battle;
    Compare compare;
    Spsa rng;
public:
    void start();
};

