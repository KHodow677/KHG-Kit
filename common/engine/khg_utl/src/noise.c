#include <stdlib.h>
#include <math.h>
#include "khg_utl/noise.h"

float utl_noise_interpolation_cosine(float a, float b, float x){
  float ft = x * M_PI;
  float f = (1 - cos(ft)) * .5;
  return a * (1 - f) + b * f;
}

void utl_noise_interpolation_2d(float **map, int start_y, int start_x, int end_y, int end_x){
  for (int i = start_y; i <= end_y; i++){
    map[i][start_x] = utl_noise_interpolation_cosine(map[start_y][start_x], map[end_y][start_x], (float)(i - start_y) / (end_y - start_y));
    map[i][end_x] = utl_noise_interpolation_cosine(map[start_y][end_x], map[end_y][end_x], (float)(i - start_y) / (end_y - start_y));
    for (int j = start_x; j <= end_x; j++){
      map[i][j] = utl_noise_interpolation_cosine(map[i][start_x], map[i][end_x], (float)(j - start_x) / (end_x - start_x));
    }
  }
}

void utl_noise_normalize_noise(float ** map, int size){
  float maxi = map[0][0];
  float mini = map[0][0];
  for (unsigned int i = 0; i < size; i++){
    for (unsigned int j = 0; j < size; j++){
      if (map[i][j] > maxi){
        maxi = map[i][j];
      }
      if (map[i][j] < mini){
        mini = map[i][j];
      }
    }
  }
  float dif = maxi - mini;
  for (unsigned int i = 0; i < size; i++){
    for (unsigned int j = 0; j < size; j++){
      map[i][j] = (map[i][j] - mini)/dif;
    }
  }
}

float **utl_noise_value_noise(float **noise_map, int size, int octaves, float scaling_factor){
  float **gen_map = (float **)calloc(size, sizeof(float *));
  for (unsigned int i = 0; i < size; i ++){
    gen_map[i] = (float *)calloc(size, sizeof(float));
  }
  int pitch;
  float persistance = 1;
  for (unsigned int o = 0; o <= octaves; o++){
    pitch = (int)(size - 1)/pow(2, o);
    for (int i = 0; i < size; i += pitch){
      for (int j = 0; j < size; j += pitch){
        gen_map[i][j] += noise_map[i][j] * persistance;
      }
    }
    for (int i = 0; i < size - pitch; i += pitch){
      for (int j = 0; j < size - pitch; j += pitch){
        utl_noise_interpolation_2d(gen_map, i, j, i + pitch, j + pitch);
      }
    }
    persistance /= scaling_factor;
  }
  utl_noise_normalize_noise(gen_map, size);
  return gen_map;
}

float **utl_noise_generate_value_noise(int seed, int size, int octaves, float scaling_factor){
  srand(seed);
  float ** nmap = (float **)calloc(size, sizeof(float *));
  for (unsigned int i = 0; i < size; i++){
    nmap[i] = (float *)calloc(size, sizeof(float));
    for (unsigned int j = 0; j < size; j++){
      nmap[i][j] = rand() % 10001 / 1000.;
    }
  }
  float **gmap = utl_noise_value_noise(nmap, size, octaves, scaling_factor);
  for (unsigned int i = 0; i < size; i++){
    free(nmap[i]);
  }
  free(nmap);
  return gmap;
}

