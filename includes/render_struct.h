#pragma once
#include "image.h"
#include "scene.h"

// typedef void (*render_mode_f)(struct rgb_image *, struct scene *, size_t x,
// size_t y);

typedef void (*render_mode_f)(struct rgb_image *, struct scene *, size_t x,
                              size_t y);

struct render_struct
{
    render_mode_f renderer;
    struct rgb_image *image;
    struct scene *scene;
    size_t startx;
    size_t starty;
    size_t maxx;
    size_t maxy;
};

struct render_struct *alloc_render_struct(render_mode_f renderer,
                                          struct rgb_image *image,
                                          struct scene *scene, size_t startx,
                                          size_t starty, size_t maxx,
                                          size_t maxy);
