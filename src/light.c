#include "light.h"
/*
 ** The color of a light is encoded inside a float, from 0 to +inf,
 ** where 0 is no light, and +inf a lot more light. Unfortunately,
 ** regular images can't hold such a huge range, and each color channel
 ** is usualy limited to [0,255]. This function does the (lossy) translation
 ** by mapping the float [0,1] range to [0,255]
 */
uint8_t translate_light_component(double light_comp)
{
    if (light_comp < 0.)
        light_comp = 0.;
    if (light_comp > 1.)
        light_comp = 1.;

    return light_comp * 255;
}

/*
 ** Converts an rgb floating point light color to 24 bit rgb.
 */
struct rgb_pixel rgb_color_from_light(const struct vec3 *light)
{
    struct rgb_pixel res;
    res.r = translate_light_component(light->x);
    res.g = translate_light_component(light->y);
    res.b = translate_light_component(light->z);
    return res;
}
