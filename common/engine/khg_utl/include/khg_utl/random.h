#pragma once

#include <stddef.h>

void utl_random_seed_clock(void);
void utl_random_seed(unsigned int seed);
void utl_random_shuffle(void *array, unsigned int n, unsigned int size);
void utl_random_choices(void *array, unsigned int n, unsigned int size, unsigned int num_choices, void *choices, float *weights);
void utl_random_sample(void *array, unsigned int n, unsigned int size, unsigned int num_samples, void *samples);
void utl_random_get_state(unsigned int *state);
void utl_random_set_state(const unsigned int *state);

void *utl_random_choice(void *array, unsigned int n, unsigned int size);

int utl_random_randint(int a, int b);
int utl_random_randrange(int a, int b, int step);
int utl_random_randbits(int a);

float utl_random_random(void);
float utl_random_uniform(float a, float b);
float utl_random_triangular(float low, float high, float mode);
float utl_random_gauss(float mean, float stddev);
float utl_random_expo(float lambda);
float utl_random_log_normal(float mean, float stddev);
float utl_random_gamma(float shape, float scale);
float utl_random_pareto(float shape, float scale);
float utl_random_beta(float alpha, float beta);
float utl_random_weibull(float shape, float scale);
float utl_random_von_mises(float mu, float kappa);

