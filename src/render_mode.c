#include "render_mode.h"
struct vec3 *get_pix_color(struct rgb_image *image, struct scene *scene,
                           double x, double y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return NULL;

    struct material *mat = closest_intersection.material;
    struct vec3 *p = xalloc(sizeof(struct vec3));
    *p = mat->shade(mat, &closest_intersection.location, scene, &ray);
    return p;
}

void add_pix_color(struct vec3 *pix_color, struct vec3 *pix_color2)
{
    if (pix_color2)
    {
        pix_color->x += pix_color2->x;
        pix_color->y += pix_color2->y;
        pix_color->z += pix_color2->z;
    }
    free(pix_color2);
}
struct vec3 *get_final_pix_color(struct rgb_image *image, struct scene *scene,
                                 double x, double y)
{
    struct vec3 *pix_color = get_pix_color(image, scene, x, y);
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
/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found, shade the pixel to
 ** find its color.
 */
void render_shaded(struct rgb_image *image, struct scene *scene, size_t x,
                   size_t y)
{
    struct vec3 *pix_color = get_final_pix_color(image, scene, x, y);
    if (pix_color)
        rgb_image_set(image, x, y, rgb_color_from_light(pix_color));
    free(pix_color);
}

/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found, shade the pixel to
 ** find its color.
 */
void render_normals(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    rgb_image_set(image, x, y, rgb_color_from_light(&pix_color));
}
void render_perlin1(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return;

    struct material *mat = closest_intersection.material;
    struct vec3 pix_color
        = perlin_shader1(mat, &closest_intersection.location, scene, &ray);

    rgb_image_set(image, x, y, rgb_color_from_light(&pix_color));
}
void render_perlin2(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return;

    struct material *mat = closest_intersection.material;
    struct vec3 pix_color
        = perlin_shader2(mat, &closest_intersection.location, scene, &ray);

    rgb_image_set(image, x, y, rgb_color_from_light(&pix_color));
}
void render_perlin3(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return;

    struct material *mat = closest_intersection.material;
    struct vec3 pix_color
        = perlin_shader3(mat, &closest_intersection.location, scene, &ray);

    rgb_image_set(image, x, y, rgb_color_from_light(&pix_color));
}
/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found, shade the pixel to
 ** find its color.
 */
void render_distances(struct rgb_image *image, struct scene *scene, size_t x,
                      size_t y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return;

    assert(closest_intersection_dist > 0);

    double depth_repr = 1 / (closest_intersection_dist + 1);
    uint8_t depth_intensity = translate_light_component(depth_repr);
    struct rgb_pixel pix_color
        = {depth_intensity, depth_intensity, depth_intensity};
    rgb_image_set(image, x, y, pix_color);
}
