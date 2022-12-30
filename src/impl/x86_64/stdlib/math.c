#include "stdlib/math.h"

// TODO: can't support floats yet
int pow(int n, int exp)
{
    if(exp == 0)
        return 1;
    if(exp < 0)
        return 1 / pow(n, -exp);
    while(--exp)
    {
        n *= n;
    }
    return n;
}
