#include "perlin.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>

static int SEED = 1985;

static const unsigned char HASH[] = {
    208, 34,  231, 213, 32,  248, 233, 56,  161, 78,  24,  140, 71,  48,  140,
    254, 245, 255, 247, 247, 40,  185, 248, 251, 245, 28,  124, 204, 204, 76,
    36,  1,   107, 28,  234, 163, 202, 224, 245, 128, 167, 204, 9,   92,  217,
    54,  239, 174, 173, 102, 193, 189, 190, 121, 100, 108, 167, 44,  43,  77,
    180, 204, 8,   81,  70,  223, 11,  38,  24,  254, 210, 210, 177, 32,  81,
    195, 243, 125, 8,   169, 112, 32,  97,  53,  195, 13,  203, 9,   47,  104,
    125, 117, 114, 124, 165, 203, 181, 235, 193, 206, 70,  180, 174, 0,   167,
    181, 41,  164, 30,  116, 127, 198, 245, 146, 87,  224, 149, 206, 57,  4,
    192, 210, 65,  210, 129, 240, 178, 105, 228, 108, 245, 148, 140, 40,  35,
    195, 38,  58,  65,  207, 215, 253, 65,  85,  208, 76,  62,  3,   237, 55,
    89,  232, 50,  217, 64,  244, 157, 199, 121, 252, 90,  17,  212, 203, 149,
    152, 140, 187, 234, 177, 73,  174, 193, 100, 192, 143, 97,  53,  145, 135,
    19,  103, 13,  90,  135, 151, 199, 91,  239, 247, 33,  39,  145, 101, 120,
    99,  3,   186, 86,  99,  41,  237, 203, 111, 79,  220, 135, 158, 42,  30,
    154, 120, 67,  87,  167, 135, 176, 183, 191, 253, 115, 184, 21,  233, 58,
    129, 233, 142, 39,  128, 211, 118, 137, 139, 255, 114, 20,  218, 113, 154,
    27,  127, 246, 250, 1,   8,   198, 250, 209, 92,  222, 173, 21,  88,  102,
    219};
void init_perlin(void)
{
    srand(time(NULL));
    // SEED = rand();
}

int noise2(int x, int y)
{
    int yindex = ((y + SEED)) % 256;
    if (yindex < 0)
        yindex += 256;
    int xindex = (HASH[yindex] + x) % 256;
    if (xindex < 0)
        xindex += 256;
    const int result = HASH[xindex];
    return result;
}

static double lin_inter(double x, double y, double s)
{
    return x + s * (y - x);
}

static double smooth_inter(double x, double y, double s)
{
    return lin_inter(x, y, s * s * (3 - 2 * s));
}

double noise2d(double x, double y)
{
    const int x_int = (int)floor(x);
    const int y_int = (int)floor(y);
    const double x_frac = x - x_int;
    const double y_frac = y - y_int;
    const int s = noise2(x_int, y_int);
    const int t = noise2(x_int + 1, y_int);
    const int u = noise2(x_int, y_int + 1);
    const int v = noise2(x_int + 1, y_int + 1);
    const double low = smooth_inter(s, t, x_frac);
    const double high = smooth_inter(u, v, x_frac);
    const double result = smooth_inter(low, high, y_frac);
    return result;
}

double Perlin_Get2d(double x, double y, double freq, int depth)
{
    double xa = x * freq;
    double ya = y * freq;
    double amp = 1.0;
    double fin = 0;
    double div = 0.0;
    for (int i = 0; i < depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }
    return fin / div;
}

double Perlin_Get3d(double x, double y, double z, double freq, int depth)
{
    double ab = Perlin_Get2d(x, y, freq, depth);
    double bc = Perlin_Get2d(y, z, freq, depth);
    double ac = Perlin_Get2d(x, z, freq, depth);

    double ba = Perlin_Get2d(y, x, freq, depth);
    double cb = Perlin_Get2d(z, y, freq, depth);
    double ca = Perlin_Get2d(z, x, freq, depth);
    double abc = ab + bc + ac + ba + cb + ca;
    return abc / 6.0;
}

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

double grad3(int hash, double x, double y, double z)
{
    int h = hash & 15; // Convert low 4 bits of hash code into 12 simple
    double u = h < 8 ? x : y; // gradient directions, and compute dot product.
    double v = h < 4                ? y
               : h == 12 || h == 14 ? x
                                    : z; // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
double grad1(int hash, double x, double y, double z)
{
    int a = hash ^ (int)x;
    int b = hash ^ (int)y;
    int c = hash ^ (int)z;

    return sin(a + b + c);
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

double lerp(double t, double a, double b)
{
    return a + t * (b - a);
}
double fade(double x)
{
    return (x * x * x * (x * (x * 6 - 15) + 10));
}
/**
** Store the fractional part of n in fn and the integer part of n in in
**/
void separate(double n, int *in, double *fn)
{
    *in = fast_floor(n);
    *fn = n - *in;
}
double noise3(double x, double y, double z)
{
    int ix0, iy0, ix1, iy1, iz0, iz1;
    double fx0, fy0, fz0, fx1, fy1, fz1;
    double s, t, r;
    double nxy0, nxy1, nx0, nx1, n0, n1;
    separate(x, &ix0, &fx0);
    separate(y, &iy0, &fy0);
    separate(z, &iz0, &fz0);
    fx1 = fx0 - 1.0;
    fy1 = fy0 - 1.0;
    fz1 = fz0 - 1.0;
    ix1 = (ix0 + 1) & 255;
    iy1 = (iy0 + 1) & 255;
    iz1 = (iz0 + 1) & 255;
    ix0 = (ix0 % 255) & 255;
    iy0 = (iy0 % 255) & 255;
    iz0 = (iz0 % 255) & 255;

    r = fade(fz0);
    t = fade(fy0);
    s = fade(fx0);

    nxy0 = grad3(perm[ix0 + perm[iy0 + perm[iz0]]], fx0, fy0, fz0);
    nxy1 = grad3(perm[ix0 + perm[iy0 + perm[iz1]]], fx0, fy0, fz1);
    nx0 = lerp(r, nxy0, nxy1);

    nxy0 = grad3(perm[ix0 + perm[iy1 + perm[iz0]]], fx0, fy1, fz0);
    nxy1 = grad3(perm[ix0 + perm[iy1 + perm[iz1]]], fx0, fy1, fz1);
    nx1 = lerp(r, nxy0, nxy1);

    n0 = lerp(t, nx0, nx1);

    nxy0 = grad3(perm[ix1 + perm[iy0 + perm[iz0]]], fx1, fy0, fz0);
    nxy1 = grad3(perm[ix1 + perm[iy0 + perm[iz1]]], fx1, fy0, fz1);
    nx0 = lerp(r, nxy0, nxy1);

    nxy0 = grad3(perm[ix1 + perm[iy1 + perm[iz0]]], fx1, fy1, fz0);
    nxy1 = grad3(perm[ix1 + perm[iy1 + perm[iz1]]], fx1, fy1, fz1);
    nx1 = lerp(r, nxy0, nxy1);

    n1 = lerp(t, nx0, nx1);

    return 0.936 * (lerp(s, n0, n1));
}

double noise3d(double x, double y, double z, double freq, int depth)
{
    double xa = x * freq;
    double ya = y * freq;
    double za = z * freq;
    double amp = 1.0;
    double fin = 0;
    double div = 0.0;
    for (int i = 0; i < depth; i++)
    {
        div += amp;
        fin += noise3(xa, ya, za) * amp;
        amp /= 3;
        xa *= 3;
        ya *= 3;
        za *= 3;
    }
    return fin / div;
}
