#pragma once

#if defined(NAMESPACE_LOADING_IMPL) || defined(NAMESPACE_LOADING_USE)
typedef struct loading_namespace {
  int (* const mult2) (int x);
  int (* const mult3) (int x);
} loading_namespace;
#endif

#ifdef NAMESPACE_LOADING_USE
loading_namespace *NAMESPACE_LOADING(void);
#else
int mod1_mult2 (int x);
int mod1_mult3 (int x);
#endif
