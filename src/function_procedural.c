#include "function_procedural.h"
#include "perlin.h"
#include "procedural.h"

double fbm3d(double x, double y, double z, double H, int numOctaves)
{
    double G = exp2(-H);
    double f = 0.8;
    double a = 1.0;
    double t = 0.0;
    for (int i = 0; i < numOctaves; i++)
    {
        double noise = noise3(f * x, f * y, f * z);
        t += a * noise;
        f *= 2.0;
        a *= G;
    }
    return t;
}

double interference(double x, double y, double z, double H, int numOctaves)
{
    double G = exp2(-H);
    double f = 10.0;
    double a = 1.0;
    double t = 0.0;
    for (int i = 0; i < numOctaves; i++)
    {
        double noise = noise3(f * x, f * y, f * z);
        if (noise < 0.0)
            noise *= -1;
        t += a * noise;
        f *= 2.0;
        a *= G;
    }
    return t;
}

double domain_warping3d(double x, double y, double z, double H, int numOctaves)
{
    double x1 = fbm3d(x, y, z, H, numOctaves);
    double y1 = fbm3d(x + 5.2, y + 1.3, z, H, numOctaves);
    double z1 = x1;

    double x2 = fbm3d(x + 4.0 * x1 + 1.7, y + 4 * y1 + 9.2, z + 4 * z1, H,
                      numOctaves);
    double y2 = fbm3d(x + 4.0 * x1 + 8.3, y + 4 * y1 + 2.8, z + 4 * z1, H,
                      numOctaves);
    double z2 = fbm3d(x + 4.0 * x1, y + 4.0 * y1, z + 4.0 * z1, H, numOctaves);

    double final
        = fbm3d(x + 4.0 * x2, y + 4.0 * y2, z + 4.0 * z2, H, numOctaves);

    return final;
}
double ribbon(double x, double y, double z, double H, int numOctaves)
{
    double a = fbm3d(x, y, z, H, numOctaves);
    double b = fbm3d(z, y, x, H, numOctaves);
    return sin(a / b);
}

double galaxy(double x, double y, double z, double H, int numOctaves)
{
    double a = fbm3d(x, y, z, H, numOctaves);
    double b = fbm3d(z, y, x, H, numOctaves);
    return sin(b * a) / cos(b / a);
}
double galaxy_distorted(double x, double y, double z, double H, int numOctaves)
{
    double a = domain_warping3d(x, y, z, H * 9, numOctaves);
    double b = domain_warping3d(z, y, x, H * 9, numOctaves);
    return sin(b * a) / cos(b / a);
}

double pastel(double x, double y, double z, double H, int numOctaves)
{
    double a = fbm3d(x, y, z, H * 9, numOctaves);
    return erfc(a);
}
double hypnotic(double x, double y, double z, double H, int numOctaves)
{
    double a = fbm3d(x, y, z, H, numOctaves);
    return cos(tgamma(a) * 10);
}

double noise3d(double x, double y, double z, struct perlin_info p)
{
    return p.noise_function(x, y, z, p.freq, p.depth);
}
