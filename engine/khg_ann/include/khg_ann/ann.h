#pragma once

#include <stdio.h>

struct ann_ann;
typedef double (*ann_actfun)(const struct ann_ann *ann, double a);

typedef struct ann_ann {
  int inputs, hidden_layers, hidden, outputs;
  ann_actfun activation_hidden;
  ann_actfun activation_output;
  int total_weights;
  int total_neurons;
  double *weight;
  double *output;
  double *delta;
} ann_ann;

ann_ann *ann_init(int inputs, int hidden_layers, int hidden, int outputs);
ann_ann *ann_read(FILE *in);
void ann_rand(ann_ann *ann);
ann_ann *ann_copy(ann_ann const *ann);
void ann_free(ann_ann *ann);

double const *ann_run(ann_ann const *ann, double const *inputs);
void ann_train(ann_ann const *ann, double const *inputs, double const *desired_outputs, double learning_rate);
void ann_write(ann_ann const *ann, FILE *out);

void ann_init_sigmoid_lookup(const ann_ann *ann);
double ann_act_sigmoid(const ann_ann *ann, double a);
double ann_act_sigmoid_cached(const ann_ann *ann, double a);
double ann_act_threshold(const ann_ann *ann, double a);
double ann_act_linear(const ann_ann *ann, double a);

