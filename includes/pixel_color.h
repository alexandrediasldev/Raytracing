#pragma once

#include "render_mode.h"

struct vec3 *get_final_pix_color(struct rgb_image *image, struct scene *scene,
                                 double x, double y, pix_color_f get_pix_color);
