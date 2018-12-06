#pragma once
#include <sys/time.h>

inline bool operator==(struct timeval l, struct timeval r) {
  return l.tv_sec == r.tv_sec && l.tv_usec == r.tv_usec;
}

inline bool operator<(struct timeval l, struct timeval r) {
  return l.tv_sec < r.tv_sec || (l.tv_sec == r.tv_sec && l.tv_usec < r.tv_usec);
}

inline bool operator<=(struct timeval l, struct timeval r) {
  return l.tv_sec < r.tv_sec ||
         (l.tv_sec == r.tv_sec && l.tv_usec <= r.tv_usec);
}

inline bool operator>(struct timeval l, struct timeval r) {
  return l.tv_sec > r.tv_sec || (l.tv_sec == r.tv_sec && l.tv_usec > r.tv_usec);
}

inline bool operator>=(struct timeval l, struct timeval r) {
  return l.tv_sec > r.tv_sec ||
         (l.tv_sec == r.tv_sec && l.tv_usec >= r.tv_usec);
}

inline struct timeval operator+(struct timeval l, struct timeval r) {
  struct timeval tv = {l.tv_sec + r.tv_sec, l.tv_usec + r.tv_usec};
  if (tv.tv_usec > 1000000) {
    tv.tv_sec++;
    tv.tv_usec -= 1000000;
  }
  return tv;
}

inline struct timeval operator+=(struct timeval l, struct timeval r) {
  l.tv_sec += r.tv_sec;
  l.tv_usec += r.tv_usec;
  if (l.tv_usec > 1000000) {
    l.tv_sec++;
    l.tv_usec -= 1000000;
  }
  return l;
}

inline struct timeval operator-(struct timeval l, struct timeval r) {
  struct timeval tv = {l.tv_sec - r.tv_sec, l.tv_usec - r.tv_usec};
  if (tv.tv_usec < 0) {
    tv.tv_sec--;
    tv.tv_usec += 1000000;
  }
  return tv;
}

inline struct timeval operator-=(struct timeval l, struct timeval r) {
  l.tv_sec -= r.tv_sec;
  l.tv_usec -= r.tv_usec;
  if (l.tv_usec < 0) {
    l.tv_sec--;
    l.tv_usec += 1000000;
  }
  return l;
}

inline struct timeval timeval_create(uint32_t sec,
                                     uint32_t msec,
                                     uint32_t usec) {
  struct timeval t;
  t.tv_sec = sec;
  t.tv_usec = msec * 1000 + usec;
  return t;
}

inline struct timeval now() {
  struct timeval tv = {};
  gettimeofday(&tv, nullptr);
  return tv;
}

inline double toDouble(struct timeval t) {
  return (double)(t.tv_sec) + (double)(t.tv_usec) / 1000000.0;
}
