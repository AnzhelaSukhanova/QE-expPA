#include "simp_tools.h"

uint64_t max(uint64_t a, uint64_t b)
{
	return (a > b) ? a : b;
}

uint64_t max3(uint64_t a, uint64_t b, uint64_t c)
{
	return max(max(a, b), c);
}
