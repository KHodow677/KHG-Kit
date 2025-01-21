#include "khg_utl/random.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static unsigned int rand_state;

void utl_random_seed(unsigned int seed) {
  rand_state = seed;
  srand(seed);
}

int utl_random_randint(int a, int b) {
  if (a > b) {
    int temp = a;
    a = b;
    b = temp;
  }
  int result = a + rand() % (b - a + 1);
  return result;
}

int utl_random_randrange(int a, int b, int step) {
  if (a == b || step == 0) {
    utl_error_func("Invalid range or step", utl_user_defined_data);
    return -1;
  }
  if ((step > 0 && a > b) || (step < 0 && a < b)) {
    utl_error_func("Logic error with range and step values", utl_user_defined_data);
    return -1;
  }
  int range = abs(b - a);
  int num_steps = (range + abs(step) - 1) / abs(step);
  int random_step = rand() % num_steps;
  int result = a + random_step * step;
  return result;
}

float utl_random_random() {
  float result = rand() / (float)RAND_MAX;
  return result;
}

float utl_random_uniform(float a, float b) {
  if (a > b) {
    float temp = a;
    a = b;
    b = temp;
  }
  float result = a + (rand() / (float)RAND_MAX) * (b - a);
  return result;
}

int utl_random_randbits(int a) {
  if (a <= 0 || a > (int)(sizeof(int) * 8)) {
    utl_error_func("Out of valid range", utl_user_defined_data);
    return -1;
  }
  int result = 0;
  for (int i = 0; i < a; i++) {
    result = (result << 1) | (rand() & 1);
  }
  return result;
}

void utl_random_shuffle(void *array, unsigned int n, unsigned int size) {
  if (array == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return;
  }
  if (n == 0) {
    utl_error_func("Size of array is zero", utl_user_defined_data);
    return;
  }
  char *arr = (char *)array;
  for (unsigned int i = 0; i < n - 1; i++) {
    unsigned int j = i + utl_random_randint(0, n - i - 1);
    void *temp = malloc(size);
    if (temp == NULL) {
      utl_error_func("Memory allocation failed", utl_user_defined_data);
      return;
    }
    memcpy(temp, arr + j * size, size);
    memcpy(arr + j * size, arr + i * size, size);
    memcpy(arr + i * size, temp, size);
    free(temp);
  }
}

void *utl_random_choice(void *array, unsigned int n, unsigned int size) {
  if (array == NULL) {
    utl_error_func("Array is NULL", utl_user_defined_data);
    return NULL;
  }
  if (n == 0) {
    utl_error_func("Size of array is zero", utl_user_defined_data);
    return NULL;
  }
  unsigned int index = utl_random_randint(0, n - 1);
  void *result = (char *)array + index * size;
  return result;
}

float utl_random_triangular(float low, float high, float mode) {
  if (low > high) {
    float temp = low;
    low = high;
    high = temp;
  }
  if (mode < low || mode > high) {
    mode = (low + high) / 2;
  }
  float u = utl_random_random();
  float c = (mode - low) / (high - low);
  float result;
  if (u <= c) {
    result = low + sqrt(u * (high - low) * (mode - low));
  }
  else {
    result = high - sqrt((1 - u) * (high - low) * (high - mode));
  }
  return result;
}

void utl_random_choices(void *array, unsigned int n, unsigned int size, unsigned int num_choices, void *choices, float *weights) {
  if (array == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return;
  }
  if (n == 0) {
    utl_error_func("Size of array is zero", utl_user_defined_data);
    return;
  }
  if (num_choices == 0) {
    utl_error_func("Number of choices is zero", utl_user_defined_data);
    return;
  }
  if (weights == NULL) {
    utl_error_func("Weights array is null", utl_user_defined_data);
    return;
  }
  float *cumulative_weights = malloc(n * sizeof(float));
  if (cumulative_weights == NULL) {
    utl_error_func("Memory allocation failed for cumulative_weights", utl_user_defined_data);
    return;
  }
  cumulative_weights[0] = weights[0];
  for (unsigned int i = 1; i < n; i++) {
    cumulative_weights[i] = cumulative_weights[i - 1] + weights[i];
  }
  for (unsigned int i = 0; i < num_choices; i++) {
    float r = utl_random_uniform(0.0, cumulative_weights[n - 1]);
    unsigned int index = 0;
    while (index < n - 1 && r > cumulative_weights[index]) {
      index++;
    }
    memcpy((char *)choices + i * size, (char *)array + index * size, size);
  }
  free(cumulative_weights);
}

void utl_random_sample(void *array, unsigned int n, unsigned int size, unsigned int num_samples, void *samples) {
  if (array == NULL) {
    utl_error_func("Array is null", utl_user_defined_data);
    return;
  }
  if (n == 0) {
    utl_error_func("Size of array is zero", utl_user_defined_data);
    return;
  }
  if (num_samples == 0) {
    utl_error_func("Number of samples is zero", utl_user_defined_data);
    return;
  }
  if (num_samples > n) {
    utl_error_func("Number of samples is greater than the number of elements in array", utl_user_defined_data);
    return;
  }
  unsigned int *indices = malloc(n * sizeof(unsigned int));
  if (indices == NULL) {
    utl_error_func("Memory allocation failed for indices", utl_user_defined_data);
    return;
  }
  for (unsigned int i = 0; i < n; i++) {
    indices[i] = i;
  }
  for (unsigned int i = 0; i < num_samples; i++) {
    unsigned int j = i + utl_random_randint(0, n - i - 1);
    unsigned int temp = indices[i];
    indices[i] = indices[j];
    indices[j] = temp;
  }
  for (unsigned int i = 0; i < num_samples; i++) {
      memcpy((char *)samples + i * size, (char *)array + indices[i] * size, size);
  }
  free(indices);
}

void utl_random_set_state(const unsigned int *state) {
  if (state != NULL) {
    rand_state = *state;
    srand(rand_state);
  } 
  else {
    utl_error_func("State is null", utl_user_defined_data);
  }
}

void utl_random_get_state(unsigned int *state) {
  if (state != NULL) {
    *state = rand_state;
  } 
  else {
    utl_error_func("State is null", utl_user_defined_data);
  }
}

float utl_random_gauss(float mean, float stddev) {
  static int hasSpare = 0;
  static float spare;
  if (hasSpare) {
    hasSpare = 0;
    float result = mean + stddev * spare;
    return result;
  }
  hasSpare = 1;
  float u, v, s;
  do {
    u = utl_random_random() * 2.0 - 1.0;
    v = utl_random_random() * 2.0 - 1.0;
    s = u * u + v * v;
  } while (s >= 1.0 || s == 0.0);
  s = sqrt(-2.0 * log(s) / s);
  spare = v * s;
  float result = mean + stddev * (u * s);
  return result;
}

float utl_random_expo(float lambda) {
  if (lambda <= 0.0) {
    utl_error_func("Lambda is less than or equal to 0", utl_user_defined_data);
    return NAN;
  }
  float expo;
  do {
    expo = utl_random_random();
  } while (expo == 0.0);
  float result = -log(expo) / lambda;
  return result;
}

float utl_random_log_normal(float mean, float stddev) {
  float normal_value = utl_random_gauss(mean, stddev);
  float result = exp(normal_value);
  return result;
}

float utl_random_gamma(float shape, float scale) {
  if (shape <= 0.0 || scale <= 0.0) {
    utl_error_func("Shape or scale is less than or equal to 0", utl_user_defined_data);
    return NAN;
  }
  if (shape < 1.0) {
    float expo;
    do {
      expo = utl_random_random();
    } while (expo == 0.0);
    float result = utl_random_gamma(1.0 + shape, scale) * pow(expo, 1.0 / shape);
    return result;
  }
  float d = shape - 1.0 / 3.0;
  float c = 1.0 / sqrt(9.0 * d);
  float v;
  while (1) {
    float u = utl_random_random();
    float z = utl_random_gauss(0.0, 1.0);
    v = pow(1.0 + c * z, 3);
    if (u < 1.0 - 0.0331 * (z * z) * (z * z)) {
      break;
    }
    if (log(u) < 0.5 * z * z + d * (1.0 - v + log(v))) {
      break;
    }
  }
  float result = d * v * scale;
  return result;
}

float utl_random_beta(float alpha, float beta) {
  if (alpha <= 0.0 || beta <= 0.0) {
    utl_error_func("Alpha or beta is less than or equal to 0", utl_error_func);
    return NAN;
  }
  float x = utl_random_gamma(alpha, 1.0);
  float y = utl_random_gamma(beta, 1.0);
  float result = x / (x + y);
  return result;
}


float utl_random_pareto(float shape, float scale) {
  if (shape <= 0.0 || scale <= 0.0) {
    utl_error_func("Shape or scale is less than or equal to 0", utl_user_defined_data);
    return NAN;
  }
  float expo;
  do {
    expo = utl_random_random();
  } while (expo == 0.0);
  float result = scale * pow((1.0 / expo), (1.0 / shape));
  return result;
}

float utl_random_weibull(float shape, float scale) {
  if (shape <= 0.0 || scale <= 0.0) {
    utl_error_func("Shape or scale is less than or equal to 0", utl_user_defined_data);
    return NAN;
  }
  float expo;
  do {
    expo = utl_random_random();
  } while (expo == 0.0);
  float result = scale * pow(-log(expo), 1.0 / shape);
  return result;
}

float utl_random_von_mises(float mu, float kappa) {
  if (kappa <= 0.0) {
    utl_error_func("Kappa is less than or equal to 0", utl_user_defined_data);
    return NAN;
  }
  const float tau = 2 * M_PI;
  float r = 1 + sqrt(1 + 4 * kappa * kappa);
  float rho = (r - sqrt(2 * r)) / (2 * kappa);
  float s = (1 + rho * rho) / (2 * rho);
  float u, w, y, z, v;
  do {
    u = utl_random_random();
    z = cos(M_PI * u);
    w = (1 + s * z) / (s + z);
    v = utl_random_random();
  } while (kappa * (s - w) - log(4 * v * (s - 1)) < 0);

  y = 2 * utl_random_random() - 1;
  if (y < 0) {
    w = -w;
  }
  float result = fmod(mu + acos(w), tau);
  return result;
}

