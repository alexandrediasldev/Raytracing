#include "procedural.h"
#include "perlin.h"
#include "utils/alloc.h"
#include <stdlib.h>

struct procedural_info *alloc_procedural_info(const struct scene *scene,
                                              double freq, double depth)
{
    struct procedural_info *p = xalloc(sizeof(struct procedural_info));
    p->freq = freq;
    p->depth = depth;
    p->abs = scene->abs;
    p->noise_function = scene->noise_function;
    return p;
}
struct vec3 procedural_texture1(struct vec3 point, struct vec3 normal,
                                struct procedural_info p)
{
    (void)normal;
    struct vec3 res;
    double noise1 = noise3d(point.x, point.y, point.z, p);

    if (p.abs && noise1 < 0.0)
        noise1 *= -1;

    res.x = noise1;
    res.y = noise1;
    res.z = noise1;
    return res;
}

struct vec3 procedural_texture2(struct vec3 point, struct vec3 normal,
                                struct procedural_info p)
{
    struct vec3 res;
    double noise_a = noise3d(point.x, point.y, point.z, p);
    double noise_b = noise3d(normal.x, normal.y, normal.z, p);

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

struct vec3 procedural_texture3(struct vec3 point, struct vec3 normal,
                                struct procedural_info p)
{
    struct vec3 res;
    double noise_a1 = noise3d(point.x, point.y, point.z, p);
    double noise_a2 = noise3d(normal.x, normal.y, normal.z, p);
    double noise_b1 = noise3d(point.y, point.z, point.x, p);
    double noise_b2 = noise3d(normal.y, normal.z, normal.x, p);
    double noise_c1 = noise3d(point.z, point.y, point.x, p);
    double noise_c2 = noise3d(normal.z, normal.y, normal.x, p);

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
struct vec3 procedural_shader1(const struct material *base_material,
                               const struct intersection *inter,
                               const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    // 4 5 0
    struct procedural_info *p = alloc_procedural_info(scene, 0.8, 2.0);
    struct vec3 procedural
        = procedural_texture1(inter->point, inter->normal, *p);
    free(p);
    return procedural;
}
struct vec3 procedural_shader2(const struct material *base_material,
                               const struct intersection *inter,
                               const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    struct procedural_info *p = alloc_procedural_info(scene, 0.5, 1);
    struct vec3 procedural
        = procedural_texture2(inter->point, inter->normal, *p);
    free(p);
    return procedural;
}

struct vec3 procedural_shader3(const struct material *base_material,
                               const struct intersection *inter,
                               const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    struct procedural_info *p = alloc_procedural_info(scene, 3.0, 2.0);
    struct vec3 procedural
        = procedural_texture3(inter->point, inter->normal, *p);
    free(p);
    return procedural;
}
