#ifndef COURSE_SIMP_TOOLS_H
#define COURSE_SIMP_TOOLS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

uint64_t gcf(uint64_t n1, uint64_t n2);
uint64_t lcm(uint64_t n1, uint64_t n2);
uint64_t max(uint64_t a, uint64_t b);
uint64_t max3(uint64_t a, uint64_t b, uint64_t c);
int l2(uint64_t n);
double get_time();

#endif //COURSE_SIMP_TOOLS_H
