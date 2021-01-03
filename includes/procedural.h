#pragma once
#include "scene.h"
#include "vec3.h"

typedef double(noise_f(double, double, double, double, int));

double fbm3d(double x, double y, double z, double H, int numOctaves);
double interference(double x, double y, double z, double H, int numOctaves);
double domain_warping3d(double x, double y, double z, double H, int numOctaves);

double ribbon(double x, double y, double z, double H, int numOctaves);

double galaxy(double x, double y, double z, double H, int numOctaves);

double galaxy_distorted(double x, double y, double z, double H, int numOctaves);

double pastel(double x, double y, double z, double H, int numOctaves);

double hypnotic(double x, double y, double z, double H, int numOctaves);

double noise3d(double x, double y, double z, double H, int numOctaves);

struct perlin_info
{
    // size of the pattern
    double freq;
    // bluriness of the pattern
    double depth;
    // absolute perlin noise or not
    int abs;
};
struct vec3 perlin_shader1(const struct material *base_material,
                           const struct intersection *inter,
                           const struct scene *scene, const struct ray *ray);

struct vec3 perlin_shader2(const struct material *base_material,
                           const struct intersection *inter,
                           const struct scene *scene, const struct ray *ray);

struct vec3 perlin_shader3(const struct material *base_material,
                           const struct intersection *inter,
                           const struct scene *scene, const struct ray *ray);
