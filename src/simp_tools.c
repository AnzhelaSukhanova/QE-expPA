#include "simp_tools.h"

uint64_t
gcf(uint64_t n1, uint64_t n2)
{
	uint64_t div;
	if (n1 == n2)
		return n1;
	int64_t dif = n1 - n2;
	if (dif < 0) {
		dif = -dif;
		div = gcf(n1, dif);
	} else
		div = gcf(n2, dif);
	return div;
}

uint64_t
lcm(uint64_t n1, uint64_t n2)
{
	return (n1*n2)/gcf(n1, n2);
}

int64_t max(int64_t a, int64_t b)
{
	return (a > b) ? a : b;
}

int64_t max3(int64_t a, int64_t b, int64_t c)
{
	return max(max(a, b), c);
}

int
l2(uint64_t n)
{
	int power = 0, val = 1;
	while (val*2 < n)
	{
		power += 1;
		val *= 2;
	}
	return power;
}

double get_time()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1.0e3 + t.tv_usec/1.0e3;
}
