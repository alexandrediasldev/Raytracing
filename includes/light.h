#pragma once
#include "image.h"
#include "scene.h"
uint8_t translate_light_component(double light_comp);

struct rgb_pixel rgb_color_from_light(const struct vec3 *light);
