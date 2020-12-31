#include "ray_calculate.h"
struct ray image_cast_ray(const struct rgb_image *image,
                          const struct scene *scene, size_t x, size_t y)
{
    // find the position of the current pixel in the image plane
    // camera_cast_ray takes camera relative positions, from -0.5 to 0.5 for
    // both axis
    double cam_x = ((double)x / image->width) - 0.5;
    double cam_y = ((double)y / image->height) - 0.5;

    // find the starting point and direction of this ray
    struct ray ray;
    camera_cast_ray(&ray, &scene->camera, cam_x, cam_y);
    return ray;
}

double scene_intersect_ray(struct object_intersection *closest_intersection,
                           struct scene *scene, struct ray *ray)
{
    // we will now try to find the closest object in the scene
    // intersecting this ray
    double closest_intersection_dist = INFINITY;

    for (size_t i = 0; i < object_vect_size(&scene->objects); i++)
    {
        struct object *obj = object_vect_get(&scene->objects, i);
        struct object_intersection intersection;
        // if there's no intersection between the ray and this object, skip it
        double intersection_dist = obj->intersect(&intersection, obj, ray);
        if (intersection_dist >= closest_intersection_dist)
            continue;

        closest_intersection_dist = intersection_dist;
        *closest_intersection = intersection;
    }

    return closest_intersection_dist;
}
