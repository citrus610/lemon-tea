#include "sprt.h"

double Sprt::elo_diff_to_win_rate(double elo_diff)
{
    return 1.0 / (1.0 + pow(10.0, (double)(-elo_diff) / 400.0));
};

double Sprt::win_rate_to_elo_diff(double win_rate)
{
    if (win_rate == 0.0) {
        return 0.0;
    }
    return -400.0 * log10(1.0 / win_rate - 1.0);
};

double Sprt::win_rate_to_elo_error_margin(double win, double draw, double loss)
{
    double total = win + draw + loss;
    double win_rate = (win + draw / 2.0) / total;

    double win_p = win / total;
    double draw_p = draw / total;
    double loss_p = loss / total;
    
    double win_dev = win_p * pow(1.0 - win_rate, 2);
    double draw_dev = draw_p * pow(0.5 - win_rate, 2);
    double loss_dev = loss_p * pow(0.0 - win_rate, 2);
    double std_dev = sqrt(win_dev + draw_dev + loss_dev) / sqrt(total);

    double confidence_p = 0.95;
    double min_confidence_p = (1.0 - confidence_p) / 2.0;
    double max_confidence_p = 1.0 - min_confidence_p;
    double min_dev = win_rate + Sprt::phi_inverse(min_confidence_p) * std_dev;
    double max_dev = win_rate + Sprt::phi_inverse(max_confidence_p) * std_dev;

    double difference = Sprt::win_rate_to_elo_diff(max_dev) - Sprt::win_rate_to_elo_diff(min_dev);

    return difference / 2.0;
};

double Sprt::phi_inverse(double x)
{
    return sqrt(2.0) * Sprt::inverse_error(2.0 * x - 1.0);
};

double Sprt::inverse_error(double x)
{
    double pi = 3.141592654;
    double a = 8.0 * (pi - 3.0) / (3.0 * pi * (4.0 - pi));
    double y = log(1.0 - x * x);
    double z = 2.0 / (pi * a) + y / 2.0;

    double result = sqrt(sqrt(z * z - y / a) - z);

    if (x < 0) {
        return -result;
    }

    return result;
};

double Sprt::log_likelihood_ratio_approximate(double win, double draw, double loss, double elo_diff_0, double elo_diff_1)
{
    if (win == 0 || loss == 0) {
        return 0.0;
    }

    double match_count = win + draw + loss;

    double win_rate = (win + draw / 2.0) / match_count;

    double win_rate_doubleiance = ((win + draw / 4.0) / match_count - win_rate * win_rate) / match_count;

    double win_rate_0 = elo_diff_to_win_rate(elo_diff_0);
    double win_rate_1 = elo_diff_to_win_rate(elo_diff_1);

    return (win_rate_1 - win_rate_0) * (2 * win_rate - win_rate_0 - win_rate_1) / win_rate_doubleiance / 2.0;
};

SPRT_RESULT Sprt::sprt(double win, double draw, double loss, double elo_diff_0, double elo_diff_1, double false_positive_rate, double false_negative_rate)
{
    double log_likelihood_ratio = Sprt::log_likelihood_ratio_approximate(win, draw, loss, elo_diff_0, elo_diff_1);

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