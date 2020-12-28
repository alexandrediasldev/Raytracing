#pragma once
#include "normal_material.h"
#include "procedural.h"
#include "render_struct.h"
#include "render_pixel.h"
#include "perlin.h"
#include "ray_calculate.h"
void render_shaded(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y);
void render_normals(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y);
void render_procedural(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y);
void render_distances(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y);


