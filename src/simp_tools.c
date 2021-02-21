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

uint64_t
GCF(uint64_t n1, uint64_t n2)
{
    int div;
    if (n1 == n2)
        return n1;
    int dif = n1 - n2;
    if (dif < 0) {
        dif = -dif;
        div = GCF(n1, dif);
    } else
        div = GCF(n2, dif);
    return div;
}
