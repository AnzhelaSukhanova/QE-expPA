#include "simp_tools.h"

void
swap(int *arr, int a, int b)
{
    int c = arr[a];
    arr[a] = arr[b];
    arr[b] = c;
}

bool
next_permutation(int *perm, int size)
{
    int j = size - 2;
    while (j != -1 && perm[j] >= perm[j + 1])
        j--;
    if (j == -1)
        return false;
    int k = size - 1;
    while (perm[j] >= perm[k])
        k--;
    swap(perm, j, k);
    int l = j + 1, r = size - 1;
    while (l < r)
        swap(perm, l++, r--);
    return true;
}
