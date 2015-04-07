#include "common.h"

int randint(int a, int b)
{
    return rand()%(b - a + 1) + a;
}
