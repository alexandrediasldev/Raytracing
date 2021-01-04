#pragma once
#include "scene.h"
#include "vec3.h"

struct procedural_info
{
    // size of the pattern
    double freq;
    // bluriness of the pattern
    double depth;
    // absolute procedural noise or not
    int abs;
    // function to use to manipulate the noise
    noise_f *noise_function;
};
struct vec3 procedural_shader1(const struct material *base_material,
                               const struct intersection *inter,
                               const struct scene *scene,
                               const struct ray *ray);

struct vec3 procedural_shader2(const struct material *base_material,
                               const struct intersection *inter,
                               const struct scene *scene,
                               const struct ray *ray);

struct vec3 procedural_shader3(const struct material *base_material,
                               const struct intersection *inter,
                               const struct scene *scene,
                               const struct ray *ray);
double noise3d(double x, double y, double z, struct procedural_info p);
