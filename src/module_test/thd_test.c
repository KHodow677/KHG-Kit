#include "thd_test.h"
#include "khg_thd/thread.h"
#include <stdio.h>
#include <assert.h>

#define BUFSIZE 128
#define NCONSUMER 5
#define NLOOPS 10

static void verbose_printf(char const* buf) {
  printf("%s", buf);
}

typedef struct _data {
  thd_mutex mutex;
  thd_condition condwrite;      
  thd_condition condread;       
  char occupied; 
  char buffer[BUFSIZE];
} t_data;

static void func_producer(t_data* t) {
  size_t i;
  thd_mutex_lock(&t->mutex);
  verbose_printf("func_producer wait...\n");
  while(t->occupied) {
    thd_condition_wait(&t->condwrite, &t->mutex);
  }
  assert(!t->occupied);
  verbose_printf("func_producer run...\n");
  for(i = 0; i < BUFSIZE; i++) {
    t->buffer[i] = i;
  }
  t->occupied = NCONSUMER;
  verbose_printf("func_producer signal\n");
  thd_condition_signal(&t->condread);
  thd_mutex_unlock(&t->mutex);
}

static void func_consumer(t_data* t) {
  size_t i;
  thd_mutex_lock(&t->mutex);
  verbose_printf("func_consumer wait...\n");
  while(!t->occupied) {
    thd_condition_wait(&t->condread, &t->mutex);
  }
  assert(t->occupied);
  verbose_printf("func_consumer run...\n");
  for(i = 0; i < BUFSIZE; i++) {
    assert(t->buffer[i] == (char)i);
  }
  t->occupied--;
  verbose_printf("func_consumer signal\n");
  if(t->occupied) {
    thd_condition_signal(&t->condread);
  }
  else {
    thd_condition_signal(&t->condwrite);
  }
  thd_mutex_unlock(&t->mutex);
}

int thd_test() {
  size_t i, j;
  t_data data;
  thd_thread  producer;
  thd_thread  consumers[NCONSUMER];
  data.occupied = 0;
  printf("test thread... ");
  thd_mutex_init(&data.mutex);
  thd_condition_init(&data.condread);
  thd_condition_init(&data.condwrite);
  for(j = 0; j < NLOOPS; j++) {
    for(i = 0; i < BUFSIZE; i++) {
      data.buffer[i] = 0;
    }
    for(i = 0; i < NCONSUMER; i++) {
      thd_thread_detach(consumers+i, (thd_thread_method)func_consumer, &data);
    }
    thd_thread_detach(&producer, (thd_thread_method)func_producer, &data);
    thd_thread_join(&producer);
    for(i = 0; i < NCONSUMER; i++) {
      thd_thread_join(consumers+i);
    }
  }
  thd_condition_destroy(&data.condread);
  thd_condition_destroy(&data.condwrite);
  thd_mutex_destroy(&data.mutex);
  printf("ok\n");
  return 0;
}

