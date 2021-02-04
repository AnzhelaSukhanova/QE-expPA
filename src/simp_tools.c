#include "simp_tools.h"

void
swap(int *a, int *b)
{
    int c;
    c = *a;
    *a = *b;
    *b = c;
}

int
factorial(int n)
{
    return (n==1 || n==0) ? 1: n * factorial(n - 1);
}

void
permute(int start, int size, int num, int sets[])
{
    if (start != size)
    {
        for (int i = start; i < size; i++)
        {
            swap(sets + num * size + start, sets + num * size + i);
            num++;
            permute(start + 1, size, num, sets);
        }
    }
}