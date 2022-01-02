#pragma once

#include <math.h>

enum SPRT_RESULT
{
    SPRT_H0,
    SPRT_H1,
    SPRT_NULL
};

static double elo_diff_to_win_rate(double elo_diff);

static double log_likelihood_ratio_approximate(double win, double draw, double loss, double elo_diff_0, double elo_diff_1);

static SPRT_RESULT sprt(double win, double draw, double loss, double elo_diff_0, double elo_diff_1, double false_positive_rate, double false_negative_rate);