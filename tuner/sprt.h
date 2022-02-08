#pragma once

#include <math.h>

enum SPRT_RESULT
{
    SPRT_H0,
    SPRT_H1,
    SPRT_NULL
};

class Sprt
{
public:
    static double elo_diff_to_win_rate(double elo_diff);
    static double win_rate_to_elo_diff(double win_rate);
    static double win_rate_to_elo_error_margin(double win, double draw, double loss);
    static double log_likelihood_ratio_approximate(double win, double draw, double loss, double elo_diff_0, double elo_diff_1);
    static double phi_inverse(double x);
    static double inverse_error(double x);
    static SPRT_RESULT sprt(double win, double draw, double loss, double elo_diff_0, double elo_diff_1, double false_positive_rate, double false_negative_rate);
};