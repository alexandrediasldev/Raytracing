#include "render_mode.h"

/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found, shade the pixel to
 ** find its color.
 */
void render_shaded(struct rgb_image *image, struct scene *scene, size_t x,
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
        = mat->shade(mat, &closest_intersection.location, scene, &ray);
    rgb_image_set(image, x, y, rgb_color_from_light(&pix_color));
}

/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found, shade the pixel to
 ** find its color.
 */
void render_normals(struct rgb_image *image, struct scene *scene, size_t x,
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
    struct vec3 pix_color = normal_material.shade(
        mat, &closest_intersection.location, scene, &ray);
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
