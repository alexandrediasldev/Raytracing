#include "render_mode.h"
#include "pixel_color.h"

/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found,
 ** find its color.
 */
static struct vec3 *get_pix_color_shadded(struct rgb_image *image,
                                          struct scene *scene, double x,
                                          double y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return NULL;

    struct material *mat = closest_intersection.material;
    struct vec3 *pix_color = xalloc(sizeof(struct vec3));
    *pix_color = mat->shade(mat, &closest_intersection.location, scene, &ray);
    return pix_color;
}
static struct vec3 *get_pix_color_normals(struct rgb_image *image,
                                          struct scene *scene, double x,
                                          double y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return NULL;

    struct material *mat = closest_intersection.material;
    struct vec3 *pix_color = xalloc(sizeof(struct vec3));
    *pix_color = normal_material.shade(mat, &closest_intersection.location,
                                       scene, &ray);
    return pix_color;
}
static struct vec3 *get_pix_color_perlin1(struct rgb_image *image,
                                          struct scene *scene, double x,
                                          double y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return NULL;

    struct material *mat = closest_intersection.material;
    struct vec3 *pix_color = xalloc(sizeof(struct vec3));
    *pix_color
        = perlin_shader1(mat, &closest_intersection.location, scene, &ray);
    return pix_color;
}
static struct vec3 *get_pix_color_perlin2(struct rgb_image *image,
                                          struct scene *scene, double x,
                                          double y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return NULL;

    struct material *mat = closest_intersection.material;
    struct vec3 *pix_color = xalloc(sizeof(struct vec3));
    *pix_color
        = perlin_shader2(mat, &closest_intersection.location, scene, &ray);
    return pix_color;
}
static struct vec3 *get_pix_color_perlin3(struct rgb_image *image,
                                          struct scene *scene, double x,
                                          double y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return NULL;

    struct material *mat = closest_intersection.material;
    struct vec3 *pix_color = xalloc(sizeof(struct vec3));
    *pix_color
        = perlin_shader3(mat, &closest_intersection.location, scene, &ray);
    return pix_color;
}
static struct vec3 *get_pix_color_distances(struct rgb_image *image,
                                            struct scene *scene, double x,
                                            double y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return NULL;

    assert(closest_intersection_dist > 0);

    double depth_repr = 1 / (closest_intersection_dist + 1);
    struct vec3 *pix_color = xalloc(sizeof(struct vec3));
    pix_color->x = depth_repr;
    pix_color->y = depth_repr;
    pix_color->z = depth_repr;
    return pix_color;
}

/**
** render generic set the pixel color using the sub function provided
**/
static void render_generic(struct rgb_image *image, struct scene *scene,
                           size_t x, size_t y, pix_color_f get_pix_color)
{
    struct vec3 *pix_color
        = get_final_pix_color(image, scene, x, y, get_pix_color);
    if (pix_color)
        rgb_image_set(image, x, y, rgb_color_from_light(pix_color));
    free(pix_color);
}
/**
** All the render_ Call the render_generic with the right sub function
**/
void render_shaded(struct rgb_image *image, struct scene *scene, size_t x,
                   size_t y)
{
    render_generic(image, scene, x, y, get_pix_color_shadded);
}

void render_normals(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    render_generic(image, scene, x, y, get_pix_color_normals);
}
void render_perlin1(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    render_generic(image, scene, x, y, get_pix_color_perlin1);
}
void render_perlin2(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    render_generic(image, scene, x, y, get_pix_color_perlin2);
}
void render_perlin3(struct rgb_image *image, struct scene *scene, size_t x,
                    size_t y)
{
    render_generic(image, scene, x, y, get_pix_color_perlin3);
}

void render_distances(struct rgb_image *image, struct scene *scene, size_t x,
                      size_t y)
{
    render_generic(image, scene, x, y, get_pix_color_distances);
}
