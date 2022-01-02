#include "sprt.h"

static double elo_diff_to_win_rate(double elo_diff)
{
    return 1.0 / (1.0 + pow(10.0, (double)(-elo_diff) / 400.0));
};

static double log_likelihood_ratio_approximate(double win, double draw, double loss, double elo_diff_0, double elo_diff_1)
{
    if (win == 0 || draw == 0 || loss == 0) {
        return 0.0;
    }

    double match_count = win + draw + loss;

    double win_rate = (win + draw / 2.0) / match_count;

    double win_rate_variance = ((win + draw / 4.0) / match_count - win_rate * win_rate) / match_count;

    double win_rate_0 = elo_diff_to_win_rate(elo_diff_0);
    double win_rate_1 = elo_diff_to_win_rate(elo_diff_1);

    return (win_rate_1 - win_rate_0) * (2 * win_rate - win_rate_0 - win_rate_1) / win_rate_variance / 2.0;
};

static SPRT_RESULT sprt(double win, double draw, double loss, double elo_diff_0, double elo_diff_1, double false_positive_rate, double false_negative_rate)
{
    double log_likelihood_ratio = log_likelihood_ratio_approximate(win, draw, loss, elo_diff_0, elo_diff_1);

    double lower_bound = log(false_negative_rate / (1.0 - false_positive_rate));
    double upper_bound = log((1.0 - false_negative_rate) / false_positive_rate);

    if (log_likelihood_ratio >= upper_bound) {
        return SPRT_H1;
    }
    else if (log_likelihood_ratio <= lower_bound) {
        return SPRT_H0;
    }
    else {
        return SPRT_NULL;
    }
};