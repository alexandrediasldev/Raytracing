#pragma once
#include "normal_material.h"
#include "perlin.h"
#include "procedural.h"
#include "ray_calculate.h"
#include "render_pixel.h"
#include "render_struct.h"

typedef struct vec3(*pix_color_f(struct rgb_image *, struct scene *, double x,
                                 double y));

void render_shaded(struct rgb_image *image, struct scene *scene, size_t x,
                   size_t y);
void render_normals(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y);
void render_procedural1(struct rgb_image *image, struct scene *scene, size_t x,
                        size_t y);
void render_procedural2(struct rgb_image *image, struct scene *scene, size_t x,
                        size_t y);
void render_procedural3(struct rgb_image *image, struct scene *scene, size_t x,
                        size_t y);

void render_distances(struct rgb_image *image, struct scene *scene, size_t x,
                      size_t y);
