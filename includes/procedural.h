#pragma once
#include "scene.h"
#include "vec3.h"

struct perlin_info
{
    // size of the pattern
    double freq;
    // bluriness of the pattern
    double depth;
    // absolute perlin noise or not
    int abs;
};
struct vec3 procedural_shader(const struct material *base_material,
                              const struct intersection *inter,
                              const struct scene *scene, const struct ray *ray);
