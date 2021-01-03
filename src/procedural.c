#include "procedural.h"
#include "perlin.h"
#include "utils/alloc.h"
#include <stdlib.h>

struct perlin_info *alloc_perlin_info(double freq, double depth, int abs)
{
    struct perlin_info *p = xalloc(sizeof(struct perlin_info));
    p->freq = freq;
    p->depth = depth;
    p->abs = abs;
    return p;
}
struct vec3 perlin_texture1(struct vec3 point, struct vec3 normal,
                            struct perlin_info p)
{
    struct vec3 res;
    double noise1 = noise3d(point.x, point.y, point.z, p.freq, p.depth);

    if (p.abs && noise1 < 0.0)
        noise1 *= -1;

    res.x = noise1;
    res.y = noise1;
    res.z = noise1;
    return res;
}

struct vec3 perlin_texture2(struct vec3 point, struct vec3 normal,
                            struct perlin_info p)
{
    struct vec3 res;
    double noise_a = noise3d(point.x, point.y, point.z, p.freq, p.depth);
    double noise_b = noise3d(normal.x, normal.y, normal.z, p.freq, p.depth);

    if (p.abs && noise_a < 0.0)
        noise_a *= -1;
    if (p.abs && noise_b < 0.0)
        noise_b *= -1;
    double noise = (noise_a + noise_b) / 2;
    res.x = noise;
    res.y = noise;
    res.z = noise;
    return res;
}

struct vec3 perlin_texture3(struct vec3 point, struct vec3 normal,
                            struct perlin_info p)
{
    struct vec3 res;
    double noise_a1 = noise3d(point.x, point.y, point.z, p.freq, p.depth);
    double noise_a2 = noise3d(normal.x, normal.y, normal.z, p.freq, p.depth);
    double noise_b1 = noise3d(point.y, point.z, point.x, p.freq, p.depth);
    double noise_b2 = noise3d(normal.y, normal.z, normal.x, p.freq, p.depth);
    double noise_c1 = noise3d(point.z, point.y, point.x, p.freq, p.depth);
    double noise_c2 = noise3d(normal.z, normal.y, normal.x, p.freq, p.depth);

    if (p.abs && noise_a1 < 0.0)
        noise_a1 *= -1;
    if (p.abs && noise_b1 < 0.0)
        noise_b1 *= -1;
    if (p.abs && noise_c1 < 0.0)
        noise_c1 *= -1;
    if (p.abs && noise_a2 < 0.0)
        noise_a2 *= -1;
    if (p.abs && noise_b2 < 0.0)
        noise_b2 *= -1;
    if (p.abs && noise_c2 < 0.0)
        noise_c2 *= -1;
    double noise_a = (noise_a1 + noise_a2) / 2;
    double noise_b = (noise_b1 + noise_b2) / 2;
    double noise_c = (noise_c1 + noise_c2) / 2;
    res.x = noise_a;
    res.y = noise_b;
    res.z = noise_c;
    return res;
}
struct vec3 perlin_shader1(const struct material *base_material,
                           const struct intersection *inter,
                           const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    // 4 5 0
    struct perlin_info *p = alloc_perlin_info(0.8, 2.0, 0);
    struct vec3 perlin = perlin_texture1(inter->point, inter->normal, *p);
    free(p);
    return perlin;
}
struct vec3 perlin_shader2(const struct material *base_material,
                           const struct intersection *inter,
                           const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    struct perlin_info *p = alloc_perlin_info(0.5, 1, 1);
    struct vec3 perlin = perlin_texture2(inter->point, inter->normal, *p);
    free(p);
    return perlin;
}

struct vec3 perlin_shader3(const struct material *base_material,
                           const struct intersection *inter,
                           const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    struct perlin_info *p = alloc_perlin_info(3.0, 2.0, 1);
    struct vec3 perlin = perlin_texture3(inter->point, inter->normal, *p);
    free(p);
    return perlin;
}

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

double noise3d(double x, double y, double z, double H, int numOctaves)
{
    return ribbon(x, y, z, H, numOctaves);
}
