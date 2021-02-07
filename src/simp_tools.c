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

int
permute(int start, int size, int num, int sets[])
{
    if (start != size)
    {
        for (int i = start; i < size; i++)
        {
            swap(sets + start, sets + i);
            if (start + 1 < size)
                num = permute(start + 1, size, num, sets);
            else {
                for (int j = 0; j < size; j++)
                    (sets + num * size)[j] = sets[j];
                return ++num;
            }
            swap(sets + start, sets + i);
        }
    }
    return num;
}