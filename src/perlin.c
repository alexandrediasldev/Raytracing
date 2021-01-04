#include "perlin.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

/**
** lookup table for perlin noise
**/
unsigned char perm[] = {
    151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
    225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
    6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
    35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
    171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
    231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
    245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
    76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
    202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
    58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
    154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
    19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
    228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
    145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
    84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
    222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
    180, 151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233,
    7,   225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,
    190, 6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203,
    117, 35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125,
    136, 171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146,
    158, 231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,
    46,  245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,
    209, 76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159,
    86,  164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123,
    5,   202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,
    16,  58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,
    44,  154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,
    253, 19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246,
    97,  228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,
    51,  145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157,
    184, 84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205,
    93,  222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,
    156, 180};
/**
**Pick a random vector determined by the
**first 4 bits of the hash function value
**/
double grad3(int hash, double x, double y, double z)
{
    switch (hash & 0xF)
    {
    case 0x0:
        return x + y;
    case 0x1:
        return -x + y;
    case 0x2:
        return x - y;
    case 0x3:
        return -x - y;
    case 0x4:
        return x + z;
    case 0x5:
        return -x + z;
    case 0x6:
        return x - z;
    case 0x7:
        return -x - z;
    case 0x8:
        return y + z;
    case 0x9:
        return -y + z;
    case 0xA:
        return y - z;
    case 0xB:
        return -y - z;
    case 0xC:
        return y + x;
    case 0xD:
        return -y + z;
    case 0xE:
        return y - x;
    case 0xF:
        return -y - z;
    default:
        return 0;
    }
}
/**
** Normal floor function takes too much time
**
*/
int fast_floor(double x)
{
    int x_cast = x;
    if (x_cast < x)
        return x_cast;
    return x_cast - 1;
}

/**
** lineraly interpolate between a and b with the weight w
**/
double lerp(double w, double a, double b)
{
    return a + w * (b - a);
}
double fade(double x)
{
    return (x * x * x * (x * (x * 6 - 15) + 10));
}

/**
** fonction to avoid overflow and limit no to 255
*/
int round_255(int n)
{
    return n & 255;
}
/**
** Store the fractional part of n in fn and the integer part of n in in
**/
void separate_double(double n, int *in, double *fn)
{
    *in = fast_floor(n);
    *fn = n - *in;
    *in = round_255(*in);
}
int get_hashed_value(int x, int y, int z)
{
    return perm[x + perm[y + perm[z]]];
}
/**
**  Get a perlin noise using 3 coordinate
**/
double noise3(double x, double y, double z)
{
    int ix0, iy0, iz0, ix1, iy1, iz1;
    double fx0, fy0, fz0, fx1, fy1, fz1;
    double s, t, r;
    double nxy0, nxy1, nx0, nx1, n0, n1;
    // get the integer and the remainder of x y z
    separate_double(x, &ix0, &fx0);
    separate_double(y, &iy0, &fy0);
    separate_double(z, &iz0, &fz0);

    fx1 = fx0 - 1.0;
    fy1 = fy0 - 1.0;
    fz1 = fz0 - 1.0;
    ix1 = round_255(ix0 + 1);
    iy1 = round_255(iy0 + 1);
    iz1 = round_255(iz0 + 1);

    r = fade(fz0);
    t = fade(fy0);
    s = fade(fx0);

    // calculate the pseudo random gradient vector and lerp them
    nxy0 = grad3(get_hashed_value(ix0, iy0, iz0), fx0, fy0, fz0);
    nxy1 = grad3(get_hashed_value(ix0, iy0, iz1), fx0, fy0, fz1);
    nx0 = lerp(r, nxy0, nxy1);

    nxy0 = grad3(get_hashed_value(ix0, iy1, iz0), fx0, fy1, fz0);
    nxy1 = grad3(get_hashed_value(ix0, iy1, iz1), fx0, fy1, fz1);
    nx1 = lerp(r, nxy0, nxy1);

    n0 = lerp(t, nx0, nx1);

    nxy0 = grad3(get_hashed_value(ix1, iy0, iz0), fx1, fy0, fz0);
    nxy1 = grad3(get_hashed_value(ix1, iy0, iz1), fx1, fy0, fz1);
    nx0 = lerp(r, nxy0, nxy1);

    nxy0 = grad3(get_hashed_value(ix1, iy1, iz0), fx1, fy1, fz0);
    nxy1 = grad3(get_hashed_value(ix1, iy1, iz1), fx1, fy1, fz1);
    nx1 = lerp(r, nxy0, nxy1);

    n1 = lerp(t, nx0, nx1);
    // magic number for rescalling in 3d
    return (0.936 * (lerp(s, n0, n1)));
}
