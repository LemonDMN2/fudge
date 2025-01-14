#include <fudge.h>
#include <abi.h>
#include "util.h"

unsigned int util_findrowtotal(char *string, unsigned int count)
{

    unsigned int i;
    unsigned int total = 0;

    for (i = 0; i < count; i++)
    {

        if (string[i] == '\n')
            total++;

    }

    return total;

}

unsigned int util_findrowstart(char *string, unsigned int count, unsigned int row)
{

    unsigned int i;

    if (!row)
        return 0;

    for (i = 0; i < count; i++)
    {

        if (string[i] == '\n')
        {

            if (!--row)
                return i + 1;

        }

    }

    return 0;

}

unsigned int util_findrowcount(char *string, unsigned int count, unsigned int offset)
{

    unsigned int i;

    for (i = offset; i < count; i++)
    {

        if (string[i] == '\n')
            return i + 1;

    }

    return count;

}

int util_intersects(int y, int y0, int y1)
{

    return y >= y0 && y < y1;

}

int util_min(int a, int b)
{

    return (a < b) ? a : b;

}

int util_max(int a, int b)
{

    return (a > b) ? a : b;

}

int util_between(int x, int min, int max)
{

    if (x < min)
        x = min;

    if (x > max)
        x = max;

    return x;

}

