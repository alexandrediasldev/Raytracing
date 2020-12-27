#pragma once
#include "vec3.h"
#include "scene.h"

struct vec3 procedural_shader(const struct material *base_material,
                          const struct intersection *inter,
                          const struct scene *scene, const struct ray *ray);
