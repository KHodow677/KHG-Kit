#pragma once

float utl_noise_interpolation_cosine(float a, float b, float x);
void utl_noise_interpolation_2d(float **map, int start_y, int start_x, int end_y, int end_x);

void utl_noise_normalize_noise(float **map, int size);
float **utl_noise_value_noise(float **noise_map, int size, int octaves, float scaling_factor);
float **utl_noise_generate_value_noise(int seed, int size, int octaves, float scaling_factor);

