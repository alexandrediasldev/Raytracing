#pragma once
#include "render_struct.h"
#include <pthread.h>
#include <stdlib.h>
void render_all_pixel(render_mode_f renderer, struct rgb_image *image,
                      struct scene *scene);
