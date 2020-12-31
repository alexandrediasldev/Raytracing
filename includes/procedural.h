#pragma once
#include "scene.h"
#include "vec3.h"

struct vec3 procedural_shader(const struct material *base_material,
                              const struct intersection *inter,
                              const struct scene *scene, const struct ray *ray);
