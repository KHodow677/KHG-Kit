#pragma once

#include <stdbool.h>

typedef struct utl_time {
  int hour;
  int minute;
  int second;
  int msec;
} utl_time;

char *utl_time_to_string(const utl_time *t);

float utl_time_current_time_in_seconds();
float utl_time_current_time_in_microseconds();
float utl_time_diff_in_seconds(const utl_time *from, const utl_time *to);

utl_time *utl_time_create(int h, int m, int s, int ms);
utl_time *utl_time_current_time(void);
utl_time *utl_time_from_msecs_since_start_of_day(int msecs);

void utl_time_add_msecs(utl_time *t, int ms);
void utl_time_add_secs(utl_time *t, int s);
void utl_time_deallocate(utl_time *t);
void utl_time_sleep(unsigned int second);

int utl_time_hour(const utl_time *t);
int utl_time_minute(const utl_time *t);
int utl_time_msec(const utl_time *t);
int utl_time_msecs_since_start_of_day();
int utl_time_msecs_to(utl_time *from, utl_time *to);
int utl_time_second(const utl_time *t);
int utl_time_secs_to(utl_time *from, utl_time *to);

bool utl_time_is_valid_time(int h, int m, int s, int ms);
bool utl_time_is_equal(const utl_time *lhs, const utl_time *rhs);
bool utl_time_is_less_than(const utl_time *lhs, const utl_time *rhs);
bool utl_time_is_less_than_or_equal(const utl_time *lhs, const utl_time *rhs);
bool utl_time_is_greater_than(const utl_time *lhs, const utl_time *rhs);
bool utl_time_is_greater_than_or_equal(const utl_time *lhs, const utl_time *rhs);
bool utl_time_is_not_equal(const utl_time *lhs, const utl_time *rhs);
bool utl_time_set_hms(utl_time *t, int h, int m, int s, int ms);
bool utl_time_is_null(const utl_time *t);
bool utl_time_is_valid(const utl_time *t);
