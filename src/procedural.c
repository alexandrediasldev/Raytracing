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
struct vec3 perlin_texture(struct vec3 point, struct vec3 normal,
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
struct vec3 procedural_shader(const struct material *base_material,
                              const struct intersection *inter,
                              const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    struct perlin_info *p = alloc_perlin_info(1.0, 1.0, 1);
    struct vec3 perlin = perlin_texture3(inter->point, inter->normal, *p);
    free(p);
    return perlin;
}
