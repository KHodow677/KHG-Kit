#include "khg_ann/ann.h"
#include "khg_utl/error_func.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANN_LOOKUP_SIZE 4096

static const double SIGMOID_DOM_MIN = -15.0;
static const double SIGMOID_DOM_MAX = 15.0;
static double INTERVAL;
static double LOOKUP[ANN_LOOKUP_SIZE];

static double genann_act_hidden(const struct ann_ann *ann, double a) {
  return ann->activation_hidden(ann, a);
}

static double genann_act_output(const struct ann_ann *ann, double a) {
  return ann->activation_output(ann, a);
}

double ann_act_sigmoid(const ann_ann *ann, double a) {
  if (a < -45.0) {
    return 0;
  }
  if (a > 45.0) {
    return 1;
  }
  return 1.0 / (1 + exp(-a));
}

void ann_init_sigmoid_lookup(const ann_ann *ann) {
  const double f = (SIGMOID_DOM_MAX - SIGMOID_DOM_MIN) / ANN_LOOKUP_SIZE;
  int i;
  INTERVAL = ANN_LOOKUP_SIZE / (SIGMOID_DOM_MAX - SIGMOID_DOM_MIN);
  for (i = 0; i < ANN_LOOKUP_SIZE; ++i) {
    LOOKUP[i] = ann_act_sigmoid(ann, SIGMOID_DOM_MIN + f * i);
  }
}

double ann_act_sigmoid_cached(const ann_ann *ann, double a) {
  assert(!isnan(a));
  if (a < SIGMOID_DOM_MIN) {
    return LOOKUP[0];
  }
  if (a >= SIGMOID_DOM_MAX) {
    return LOOKUP[ANN_LOOKUP_SIZE - 1];
  }
  size_t j = (size_t)((a-SIGMOID_DOM_MIN)*INTERVAL+0.5);
  if (j >= ANN_LOOKUP_SIZE) {
    return LOOKUP[ANN_LOOKUP_SIZE - 1];
  }
  return LOOKUP[j];
}

double ann_act_linear(const struct ann_ann *ann, double a) {
  return a;
}

double ann_act_threshold(const struct ann_ann *ann, double a) {
  return a > 0;
}

ann_ann *ann_init(int inputs, int hidden_layers, int hidden, int outputs) {
  if (hidden_layers < 0) {
    return 0;
  }
  if (inputs < 1) {
    return 0;
  }
  if (outputs < 1) {
    return 0;
  }
  if (hidden_layers > 0 && hidden < 1) {
    return 0;
  }
  const int hidden_weights = hidden_layers ? (inputs+1) * hidden + (hidden_layers-1) * (hidden+1) * hidden : 0;
  const int output_weights = (hidden_layers ? (hidden+1) : (inputs+1)) * outputs;
  const int total_weights = (hidden_weights + output_weights);
  const int total_neurons = (inputs + hidden * hidden_layers + outputs);
  const int size = sizeof(ann_ann) + sizeof(double) * (total_weights + total_neurons + (total_neurons - inputs));
  ann_ann *ret = malloc(size);
  if (!ret) {
    return 0;
  }
  ret->inputs = inputs;
  ret->hidden_layers = hidden_layers;
  ret->hidden = hidden;
  ret->outputs = outputs;
  ret->total_weights = total_weights;
  ret->total_neurons = total_neurons;
  ret->weight = (double *)((char *)ret + sizeof(ann_ann));
  ret->output = ret->weight + ret->total_weights;
  ret->delta = ret->output + ret->total_neurons;
  ann_rand(ret);
  ret->activation_hidden = ann_act_sigmoid_cached;
  ret->activation_output = ann_act_sigmoid_cached;
  ann_init_sigmoid_lookup(ret);
  return ret;
}

ann_ann *ann_read(FILE *in) {
  int inputs, hidden_layers, hidden, outputs;
  int rc = fscanf(in, "%d %d %d %d", &inputs, &hidden_layers, &hidden, &outputs);
  errno = 0;
  if (rc < 4 || errno != 0) {
    utl_error_func("Fscanf error", utl_user_defined_data);
    return NULL;
  }
  ann_ann *ann = ann_init(inputs, hidden_layers, hidden, outputs);
  for (int i = 0; i < ann->total_weights; ++i) {
    errno = 0;
    rc = fscanf(in, " %le", ann->weight + i);
    if (rc < 1 || errno != 0) {
      utl_error_func("Fscanf error", utl_user_defined_data);
      ann_free(ann);
      return NULL;
    }
  }
  return ann;
}

ann_ann *ann_copy(ann_ann const *ann) {
  const int size = sizeof(ann_ann) + sizeof(double) * (ann->total_weights + ann->total_neurons + (ann->total_neurons - ann->inputs));
  ann_ann *ret = malloc(size);
  if (!ret) {
    return 0;
  }
  memcpy(ret, ann, size);
  ret->weight = (double *)((char *)ret + sizeof(ann_ann));
  ret->output = ret->weight + ret->total_weights;
  ret->delta = ret->output + ret->total_neurons;
  return ret;
}


void ann_rand(ann_ann *ann) {
  for (int i = 0; i < ann->total_weights; ++i) {
    double r = (((double)rand())/RAND_MAX);
    ann->weight[i] = r - 0.5;
  }
}

void ann_free(ann_ann *ann) {
  free(ann);
}


double const *ann_run(ann_ann const *ann, double const *inputs) {
  double const *w = ann->weight;
  double *o = ann->output + ann->inputs;
  double const *i = ann->output;
  memcpy(ann->output, inputs, sizeof(double) * ann->inputs);
  int h, j, k;
  if (!ann->hidden_layers) {
    double *ret = o;
    for (j = 0; j < ann->outputs; ++j) {
      double sum = *w++ * -1.0;
      for (k = 0; k < ann->inputs; ++k) {
        sum += *w++ * i[k];
      }
      *o++ = genann_act_output(ann, sum);
    }
    return ret;
  }
  for (j = 0; j < ann->hidden; ++j) {
    double sum = *w++ * -1.0;
    for (k = 0; k < ann->inputs; ++k) {
      sum += *w++ * i[k];
    }
    *o++ = genann_act_hidden(ann, sum);
  }
  i += ann->inputs;
  for (int h = 1; h < ann->hidden_layers; ++h) {
    for (j = 0; j < ann->hidden; ++j) {
      double sum = *w++ * -1.0;
      for (k = 0; k < ann->hidden; ++k) {
        sum += *w++ * i[k];
      }
      *o++ = genann_act_hidden(ann, sum);
    }
    i += ann->hidden;
  }
  double const *ret = o;
  for (j = 0; j < ann->outputs; ++j) {
    double sum = *w++ * -1.0;
    for (k = 0; k < ann->hidden; ++k) {
      sum += *w++ * i[k];
    }
    *o++ = genann_act_output(ann, sum);
  }
  assert(w - ann->weight == ann->total_weights);
  assert(o - ann->output == ann->total_neurons);
  return ret;
}

void ann_train(ann_ann const *ann, double const *inputs, double const *desired_outputs, double learning_rate) {
  ann_run(ann, inputs);
  int h, j, k;
  double const *o = ann->output + ann->inputs + ann->hidden * ann->hidden_layers;
  double *d1 = ann->delta + ann->hidden * ann->hidden_layers;
  double const *t = desired_outputs;
  if (genann_act_output == ann_act_linear ||
    ann->activation_output == ann_act_linear) {
    for (j = 0; j < ann->outputs; ++j) {
      *d1++ = *t++ - *o++;
    }
  } 
  else {
    for (j = 0; j < ann->outputs; ++j) {
      *d1++ = (*t - *o) * *o * (1.0 - *o);
      ++o;
      ++t;
    }
  }
  for (h = ann->hidden_layers - 1; h >= 0; --h) {
    double const *o = ann->output + ann->inputs + (h * ann->hidden);
    double *d = ann->delta + (h * ann->hidden);
    double const * const dd = ann->delta + ((h+1) * ann->hidden);
    double const * const ww = ann->weight + ((ann->inputs+1) * ann->hidden) + ((ann->hidden+1) * ann->hidden * (h));
    for (j = 0; j < ann->hidden; ++j) {
      double delta = 0;
      for (k = 0; k < (h == ann->hidden_layers-1 ? ann->outputs : ann->hidden); ++k) {
        const double forward_delta = dd[k];
        const int windex = k * (ann->hidden + 1) + (j + 1);
        const double forward_weight = ww[windex];
        delta += forward_delta * forward_weight;
      }
      *d = *o * (1.0-*o) * delta;
      ++d;
      ++o;
    }
  }
  double const *d2 = ann->delta + ann->hidden * ann->hidden_layers;
  double *w = ann->weight + (ann->hidden_layers ? ((ann->inputs+1) * ann->hidden + (ann->hidden+1) * ann->hidden * (ann->hidden_layers-1)) : (0));
  double const * const i = ann->output + (ann->hidden_layers ? (ann->inputs + (ann->hidden) * (ann->hidden_layers-1)) : 0);
  for (j = 0; j < ann->outputs; ++j) {
    *w++ += *d2 * learning_rate * -1.0;
    for (k = 1; k < (ann->hidden_layers ? ann->hidden : ann->inputs) + 1; ++k) {
      *w++ += *d2 * learning_rate * i[k-1];
    }
    ++d2;
  }
  assert(w - ann->weight == ann->total_weights);
  for (h = ann->hidden_layers - 1; h >= 0; --h) {
    double const *d = ann->delta + (h * ann->hidden);
    double const *i = ann->output + (h ? (ann->inputs + ann->hidden * (h-1)) : 0);
    double *w = ann->weight + (h ? ((ann->inputs+1) * ann->hidden + (ann->hidden+1) * (ann->hidden) * (h-1)) : 0);
    for (j = 0; j < ann->hidden; ++j) {
      *w++ += *d * learning_rate * -1.0;
      for (k = 1; k < (h == 0 ? ann->inputs : ann->hidden) + 1; ++k) {
        *w++ += *d * learning_rate * i[k-1];
      }
      ++d;
    }
  }
}

void ann_write(ann_ann const *ann, FILE *out) {
  fprintf(out, "%d %d %d %d", ann->inputs, ann->hidden_layers, ann->hidden, ann->outputs);
  for (int i = 0; i < ann->total_weights; ++i) {
    fprintf(out, " %.20e", ann->weight[i]);
  }
}

