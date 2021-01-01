#pragma once
#include "image.h"
#include "light.h"
#include "scene.h"

double scene_intersect_ray(struct object_intersection *closest_intersection,
                           struct scene *scene, struct ray *ray);

struct ray image_cast_ray(const struct rgb_image *image,
                          const struct scene *scene, double x, double y);
