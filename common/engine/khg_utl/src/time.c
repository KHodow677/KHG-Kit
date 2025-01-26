#include "khg_utl/time.h"
#include "khg_utl/error_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else 
#include <sys/time.h>
#include <unistd.h>
#endif 

utl_time *utl_time_create(int h, int m, int s, int ms) {
  if (!utl_time_is_valid_time(h, m, s, ms)) {
    utl_error_func("Invalid time parameters", utl_user_defined_data);
    return NULL;
  }
  utl_time *my_time = (utl_time *)malloc(sizeof(utl_time));
  if (!my_time) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  my_time->hour = h;
  my_time->minute = m;
  my_time->second = s;
  my_time->msec = ms;
  return my_time;
}

utl_time *utl_time_current_time(void) {
#if defined(_WIN32) || defined(_WIN64)
  SYSTEMTIME systemTime;
  GetLocalTime(&systemTime);
  utl_time *timeObject = utl_time_create(systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
  return timeObject;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *timeinfo = localtime(&tv.tv_sec);
  utl_time *timeObject = utl_time_create(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, tv.tv_usec / 1000);
  return timeObject;
#endif
}

bool utl_time_is_valid(const utl_time *t) {
  if (t == NULL) {
    utl_error_func("Time object is null", utl_user_defined_data);
    return false;
  }
  if (t->hour < 0 || t->hour > 23) {
    return false;
  }
  if (t->minute < 0 || t->minute > 59) {
    return false;
  }
  if (t->second < 0 || t->second > 59) {
    return false;
  }
  if (t->msec < 0 || t->msec > 999) {
    return false;
  }
  return true;
}

void utl_time_add_msecs(utl_time *t, int ms) {
  if (!t) {
    utl_error_func("Null time pointer", utl_user_defined_data);
    return;
  }
  if (!utl_time_is_valid(t)) {
    utl_error_func("Invalid time object", utl_user_defined_data);
    return;
  }
  t->msec += ms;
  while (t->msec >= 1000) {
    t->msec -= 1000;
    t->second++;
  }
  while (t->second >= 60) {
    t->second -= 60;
    t->minute++;
  }
  while (t->minute >= 60) {
    t->minute -= 60;
    t->hour++;
  }
  while (t->hour >= 24) {
    t->hour -= 24;
  }
}

void utl_time_add_secs(utl_time *t, int s) {
  if (!t) {
    utl_error_func("Null time pointer", utl_user_defined_data);
    return;
  }
  if (!utl_time_is_valid(t)) {
    utl_error_func("Invalid time object", utl_user_defined_data);
    return;
  }
  t->second += s;
  while (t->second >= 60) {
    t->second -= 60;
    t->minute++;
  }
  while (t->minute >= 60) {
    t->minute -= 60;
    t->hour++;
  }
  while (t->hour >= 24) {
    t->hour -= 24;
  }
}

bool utl_time_is_null(const utl_time *t) {
  bool result = t == NULL ? true : false;
  return result;
}

int utl_time_hour(const utl_time *t) {
  if (!t) {
    utl_error_func("Null Time pointer", utl_user_defined_data);
    return -1;
  }
  if (!utl_time_is_valid(t)) {
    utl_error_func("Invalid time object", utl_user_defined_data);
    return -1;
  }
  return t->hour;
}

int utl_time_minute(const utl_time *t) {
  if (!t) {
    utl_error_func("Null time pointer", utl_user_defined_data);
    return -1;
  }
  if (!utl_time_is_valid(t)) {
    utl_error_func("Invalid time object", utl_user_defined_data);
    return -1;
  }
  return t->minute;
}

int utl_time_second(const utl_time *t) {
  if (!t) {
    utl_error_func("Null time pointer", utl_user_defined_data);
    return -1;
  }
  if (!utl_time_is_valid(t)) {
    utl_error_func("Invalid time object", utl_user_defined_data);
    return -1;
  }
  return t->second;
}

int utl_time_msec(const utl_time *t) {
  if (!t) {
    utl_error_func("Null time pointer", utl_user_defined_data);
    return -1;
  }
  if (!utl_time_is_valid(t)) {
    utl_error_func("Invalid time object", utl_user_defined_data);
    return -1;
  }
  return t->msec;
}

int utl_time_msecs_since_start_of_day() {
  utl_time *t = utl_time_current_time();
  if (!t) {
    utl_error_func("Failed to retrieve current time", utl_user_defined_data);
    return -1;
  }
  int msec_day = t->msec + (t->second * 1000) + (t->minute * 60 * 1000) + (t->hour * 60 * 60 * 1000);
  free(t);
  return msec_day;
}

int utl_time_msecs_to(utl_time *from, utl_time *to) {
  if (!from || !to) {
    utl_error_func("Null time pointers", utl_user_defined_data);
    return 0;
  }
  if (!utl_time_is_valid(from) || !utl_time_is_valid(to)) {
    utl_error_func("Invalid time objects", utl_user_defined_data);
    return 0;
  }
  int from_msecs = from->hour * 3600000 + from->minute * 60000 + from->second * 1000 + from->msec;
  int to_msecs = to->hour * 3600000 + to->minute * 60000 + to->second * 1000 + to->msec;
  int diff = to_msecs - from_msecs;
  if (diff < -86400000) {
    diff += 86400000;
  } 
  else if (diff > 86400000) {
    diff -= 86400000;
  }
  return diff;
}

int utl_time_secs_to(utl_time *from, utl_time *to) {
  if (!from || !to) {
    utl_error_func("Null time pointers", utl_user_defined_data);
    return 0;
  }
  if (!utl_time_is_valid(from) || !utl_time_is_valid(to)) {
    utl_error_func("Invalid time objects", utl_user_defined_data);
    return 0;
  }
  int from_secs = from->hour * 3600 + from->minute * 60 + from->second;
  int to_secs = to->hour * 3600 + to->minute * 60 + to->second;
  int diff = to_secs - from_secs;
  if (diff < -86400) {
    diff += 86400;
  } 
  else if (diff > 86400) {
    diff -= 86400;
  }
  return diff;
}

bool utl_time_set_hms(utl_time *t, int h, int m, int s, int ms) {
  if (!t) {
    utl_error_func("Null time pointer", utl_user_defined_data);
    return false;
  }
  if (!utl_time_is_valid_time(h, m, s, ms)) {
    utl_error_func("Invalid time parameters", utl_user_defined_data);
    return false;
  }
  t->hour = h;
  t->minute = m;
  t->second = s;
  t->msec = ms;
  return true;
}

char *utl_time_to_string(const utl_time *t) {
  if (!t) {
    utl_error_func("Null time pointer", utl_user_defined_data);
    return NULL;
  }
  if (!utl_time_is_valid(t)) {
    utl_error_func("Invalid Time object", utl_user_defined_data);
    return NULL;
  }
  char *time_str = (char *)malloc(sizeof(char) * 16);
  if (!time_str) {
    utl_error_func("Memory allocation failed", utl_user_defined_data);
    return NULL;
  }
  sprintf(time_str, "(%02d:%02d:%02d:%03d)", t->hour, t->minute, t->second, t->msec);
  return time_str;
}

bool utl_time_is_valid_time(int h, int m, int s, int ms) {
  if ((h < 0 || h > 23) || (m < 0 || m > 59) || (s < 0 || s > 59) || (ms < 0 || ms > 999)) {
    utl_error_func("Invalid time parameters", utl_user_defined_data);
    return false;
  }
  return true;
}

utl_time *utl_time_from_msecs_since_start_of_day(int msecs) {
  if (msecs < 0 || msecs >= 86400000) { // 24 hours * 60 minutes * 60 seconds * 1000 ms
    utl_error_func("Invalid milliseconds value", utl_user_defined_data);
    return NULL;
  }
  int seconds = msecs / 1000;
  int minutes = seconds / 60;
  int hours = minutes / 60;
  msecs = msecs % 1000;
  seconds = seconds % 60;
  minutes = minutes % 60;
  utl_time *my_time = utl_time_create(hours, minutes, seconds, msecs);
  if (!my_time) {
    utl_error_func("Time creation failed", utl_user_defined_data);
    return NULL;
  }
  return my_time;
}

bool utl_time_is_equal(const utl_time *lhs, const utl_time *rhs) {
  if (!lhs || !rhs) {
    utl_error_func("Null time pointers provided", utl_user_defined_data);
    return false;
  }
  if (!utl_time_is_valid(lhs) || !utl_time_is_valid(rhs)) {
    utl_error_func("Invalid time objects provided", utl_user_defined_data);
    return false;
  }
  bool result = (lhs->hour == rhs->hour && lhs->minute == rhs->minute && lhs->second == rhs->second && lhs->msec == rhs->msec);
  return result;
}

bool utl_time_is_less_than(const utl_time *lhs, const utl_time *rhs) {
  if (!lhs || !rhs) {
    utl_error_func("Null time pointers provided", utl_user_defined_data);
    return false;
  }
  if (!utl_time_is_valid(lhs) || !utl_time_is_valid(rhs)) {
    utl_error_func("Invalid time objects provided", utl_user_defined_data);
    return false;
  }
  bool result = (lhs->hour < rhs->hour) || (lhs->hour == rhs->hour && lhs->minute < rhs->minute) || (lhs->hour == rhs->hour && lhs->minute == rhs->minute && lhs->second < rhs->second) || (lhs->hour == rhs->hour && lhs->minute == rhs->minute && lhs->second == rhs->second && lhs->msec < rhs->msec);
  return result;
}

bool utl_time_is_less_than_or_equal(const utl_time *lhs, const utl_time *rhs) {
  if (!lhs || !rhs) {
    utl_error_func("Null time pointers provided", utl_user_defined_data);
    return false;
  }
  if (!utl_time_is_valid(lhs) || !utl_time_is_valid(rhs)) {
    utl_error_func("Invalid Time objects provided", utl_user_defined_data);
    return false;
  }
  bool result = utl_time_is_equal(lhs, rhs) || utl_time_is_less_than(lhs, rhs);
  return result;
}

bool utl_time_is_greater_than(const utl_time *lhs, const utl_time *rhs) {
  if (!lhs || !rhs) {
    utl_error_func("Null Time pointers provided", utl_user_defined_data);
    return false;
  }
  bool result = utl_time_is_less_than(rhs, lhs);
  return result;
}

bool utl_time_is_greater_than_or_equal(const utl_time* lhs, const utl_time* rhs) {
  if (!lhs || !rhs) {
    utl_error_func("Null time pointers provided", utl_user_defined_data);
    return false;
  }
  bool result = utl_time_is_greater_than(lhs, rhs) || utl_time_is_equal(rhs, lhs);
  return result;
}

bool utl_time_is_not_equal(const utl_time* lhs, const utl_time* rhs) {
  if (!lhs || !rhs) {
    utl_error_func("Null time pointers provided", utl_user_defined_data);
    return true; 
  }
  bool result = !utl_time_is_equal(lhs, rhs);
  return result;
}

void utl_time_deallocate(utl_time *t) {
  if (!t) {
    utl_error_func("Attempted to deallocate a null time pointer", utl_user_defined_data);
    return;
  }
  free(t);
}

float utl_time_current_time_in_seconds() {
#if defined(_WIN32) || defined(_WIN64)
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  unsigned long long time = (((unsigned long long)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
  float current_time = (float)(time / 10000000.0) - 11644473600.0;
  return current_time;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  float current_time = (float)(tv.tv_sec) + (float)(tv.tv_usec) / 1000000.0;
  return current_time;
#endif
}

float utl_time_current_time_in_microseconds() {
#if defined(_WIN32) || defined(_WIN64)
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  unsigned long long time = (((unsigned long long)ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
  float microseconds = (float)(time / 10.0) - 11644473600000000.0;
  return microseconds;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  float microseconds = (float)(tv.tv_sec) * 1000000.0 + (float)(tv.tv_usec);
  return microseconds;
#endif
}

void utl_time_sleep(unsigned int second) {
#if defined(_WIN32) || defined(_WIN64)
  Sleep(second * 1000);
#else
  sleep(second);
#endif
}

float utl_time_diff_in_seconds(const utl_time* from, const utl_time* to) {
  if (!from || !to) {
    utl_error_func("Null time pointers", utl_user_defined_data);
    return 0.0;
  }
  if (!utl_time_is_valid(from) || !utl_time_is_valid(to)) {
    utl_error_func("Invalid Time objects", utl_user_defined_data);
    return 0.0;
  }
  float from_secs = from->hour * 3600 + from->minute * 60 + from->second + from->msec / 1000.0;
  float to_secs = to->hour * 3600 + to->minute * 60 + to->second + to->msec / 1000.0;
  float diff = to_secs - from_secs;
  if (diff < -86400) {
    diff += 86400;
  } 
  else if (diff > 86400) {
    diff -= 86400;
  }
  return diff;
}
