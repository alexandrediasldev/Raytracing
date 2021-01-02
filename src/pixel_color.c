
#include "pixel_color.h"
/**
** Add two pix_color and free the second
**/
static void add_pix_color(struct vec3 *pix_color, struct vec3 *pix_color2)
{
    if (pix_color2)
    {
        pix_color->x += pix_color2->x;
        pix_color->y += pix_color2->y;
        pix_color->z += pix_color2->z;
    }
    free(pix_color2);
}
/**
** Get the final pix_color using the function given
** anti_alliasing on:
** Launch 4 rays at (x,y) (x + 0.5,y) (x,y+ 0.5) (x + 0.5,y + 0.5)
** and get the average of theses values
** anti_alliasing off:
** Launch 1 ray and get the color of the pixel
**/

struct vec3 *get_final_pix_color(struct rgb_image *image, struct scene *scene,
                                 double x, double y, pix_color_f get_pix_color)
{
    struct vec3 *pix_color = get_pix_color(image, scene, x, y);
    if (!scene->anti_alliasing)
        return pix_color;
    struct vec3 *pix_color2 = get_pix_color(image, scene, x + 0.5, y);
    struct vec3 *pix_color3 = get_pix_color(image, scene, x, y + 0.5);
    struct vec3 *pix_color4 = get_pix_color(image, scene, x + 0.5, y + 0.5);
    if (!pix_color)
        pix_color = pix_color2;
    else
        add_pix_color(pix_color, pix_color2);
    if (!pix_color)
        pix_color = pix_color3;
    else
        add_pix_color(pix_color, pix_color3);
    if (!pix_color)
        pix_color = pix_color4;
    else
        add_pix_color(pix_color, pix_color4);
    if (!pix_color)
        return NULL;
    pix_color->x /= 4;
    pix_color->y /= 4;
    pix_color->z /= 4;
    return pix_color;
}
