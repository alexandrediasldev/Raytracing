#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "bmp.h"
#include "camera.h"
#include "image.h"
#include "normal_material.h"
#include "obj_loader.h"
#include "phong_material.h"
#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "procedural.h"
#include "render_struct.h"
#include "perlin.h"
#include "vec3.h"

/*
 ** The color of a light is encoded inside a float, from 0 to +inf,
 ** where 0 is no light, and +inf a lot more light. Unfortunately,
 ** regular images can't hold such a huge range, and each color channel
 ** is usualy limited to [0,255]. This function does the (lossy) translation
 ** by mapping the float [0,1] range to [0,255]
 */
static inline uint8_t translate_light_component(double light_comp)
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

static void build_test_scene(struct scene *scene, double aspect_ratio)
{
    // create a sample red material
    struct phong_material *red_material = zalloc(sizeof(*red_material));
    phong_material_init(red_material);
    red_material->surface_color = (struct vec3){0.75, 0.125, 0.125};
    red_material->diffuse_Kn = 0.2;
    red_material->spec_n = 10;
    red_material->spec_Ks = 0.2;
    red_material->ambient_intensity = 0.1;

    // create a single sphere with the above material, and add it to the scene
    struct sphere *sample_sphere
        = sphere_create((struct vec3){0, 10, 0}, 4, &red_material->base);
    object_vect_push(&scene->objects, &sample_sphere->base);

    // go the same with a triangle
    // points are listed counter-clockwise
    //     a
    //    /|
    //   / |
    //  b--c
    struct vec3 points[3] = {
        {6, 10, 1}, // a
        {5, 10, 0}, // b
        {6, 10, 0}, // c
    };

    struct triangle *sample_triangle
        = triangle_create(points, &red_material->base);
    object_vect_push(&scene->objects, &sample_triangle->base);

    // setup the scene lighting
    scene->light_intensity = 5;
    scene->light_color = (struct vec3){1, 1, 0}; // yellow
    scene->light_direction = (struct vec3){-1, 1, -1};
    vec3_normalize(&scene->light_direction);

    // setup the camera
    double cam_width = 10;
    double cam_height = cam_width / aspect_ratio;

    scene->camera = (struct camera){
        .center = {0, 0, 0},
            .forward = {0, 1, 0},
            .up = {0, 0, 1},
            .width = cam_width,
            .height = cam_height,
            .focal_distance = focal_distance_from_fov(cam_width, 80),
    };

    // release the reference to the material
    material_put(&red_material->base);
}

static void build_obj_scene(struct scene *scene, double aspect_ratio)
{
    // setup the scene lighting
    scene->light_intensity = 5;
    scene->light_color = (struct vec3){1, 1, 0}; // yellow
    scene->light_direction = (struct vec3){-1, -1, -1};
    vec3_normalize(&scene->light_direction);

    // setup the camera
    double cam_width = 2;
    double cam_height = cam_width / aspect_ratio;

    // for some reason the object points in the z axis,
    // with its up on y
    scene->camera = (struct camera){
        .center = {0, 1, 2},
            .forward = {0, -1, -2},
            .up = {0, 1, 0},
            .width = cam_width,
            .height = cam_height,
            .focal_distance = focal_distance_from_fov(cam_width, 40),
    };

    vec3_normalize(&scene->camera.forward);
    vec3_normalize(&scene->camera.up);
}

static struct ray image_cast_ray(const struct rgb_image *image,
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

    static double
scene_intersect_ray(struct object_intersection *closest_intersection,
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
/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found, shade the pixel to
 ** find its color.
 */
static void render_shaded(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y)
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
static void render_normals(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y)
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
static void render_procedural(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y)
{
    struct ray ray = image_cast_ray(image, scene, x, y);

    struct object_intersection closest_intersection;
    double closest_intersection_dist
        = scene_intersect_ray(&closest_intersection, scene, &ray);

    // if the intersection distance is infinite, do not shade the pixel
    if (isinf(closest_intersection_dist))
        return;

    struct material *mat = closest_intersection.material;
    struct vec3 pix_color = procedural_shader(
            mat, &closest_intersection.location, scene, &ray);

    rgb_image_set(image, x, y, rgb_color_from_light(&pix_color));
}


/* For all the pixels of the image, try to find the closest object
 ** intersecting the camera ray. If an object is found, shade the pixel to
 ** find its color.
 */
static void render_distances(struct rgb_image *image, struct scene *scene,
        size_t x, size_t y)
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
void *render_line_th(void *rend_void)
{
    struct render_struct *rend = rend_void;
    size_t maxx = rend->maxx;
    size_t starty = rend->starty;
    struct rgb_image *image = rend->image;
    struct scene *scene = rend->scene;
    render_mode_f renderer = rend->renderer;
    for (size_t x = 0; x < maxx; x++)
    {
        renderer(image, scene, x, starty);
    }
    printf("rendering: %li/%li\n", starty, maxx);
    free(rend_void);
    pthread_exit(NULL);
}
void *render_line(void *rend_void)
{
    struct render_struct *rend = rend_void;
    size_t maxx = rend->maxx;
    size_t starty = rend->starty;
    struct rgb_image *image = rend->image;
    struct scene *scene = rend->scene;
    render_mode_f renderer = rend->renderer;
    for (size_t x = 0; x < maxx; x++)
    {
        renderer(image, scene, x, starty);
    }
    printf("rendering: %li/%li\n", starty, maxx);
}
void join_all(pthread_t *th, int size)
{
            for(int j = 0; j < size; j++)
                pthread_join(th[j],NULL);
}

void render_pixel(void *rend_void)
{
    struct render_struct *rend = rend_void;
    // render all pixels
    size_t starty = 0;
    size_t maxy = rend->maxy;
    int i = 0;
    for (size_t y = 0; y < maxy; y++)
    {
        render_line(rend_void);
        rend->starty++;
        i++;
    }
}

void render_pixel_th(void *rend_void)
{
    struct render_struct *rend = rend_void;
    struct rgb_image *image = rend->image;
    struct scene *scene = rend->scene;
    render_mode_f renderer = rend->renderer;
    size_t maxy = rend->maxy;
    free(rend);
    rend_void = alloc_render_struct(renderer, image, scene, 0, 0, image->height, image->width);
    // render all pixels
    int numth = 4;
    pthread_t th[numth];
    size_t starty = 0;
    int i = 0;
    for (size_t y = 0; y < maxy; y++)
    {
        if(i > numth)
        {
            join_all(th, numth);
            i = 0;
        }
        pthread_create(&th[i], NULL, render_line_th, rend_void);
        if(y != maxy -1)
        rend_void = alloc_render_struct(renderer, image, scene, 0, y, image->height, image->width);
        printf("%li\n",y);
        i++;
    }
    if(i > numth)
        join_all(th, numth);
    else
        join_all(th, i);
}
void render_all_pixel(render_mode_f renderer, struct rgb_image *image, struct scene *scene, int th)
{
    init_perlin();
            struct render_struct *rend = 0;
            rend = alloc_render_struct(renderer, image, scene, 0, 0, image->height, image->width);
            if(th)
                render_pixel_th(rend);
            else
                render_pixel(rend);


}
void time_end(clock_t begin)
{
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time_spent: %f\n",time_spent);
}
int main(int argc, char *argv[])
{
    int rc;

    if (argc < 3)
        errx(1, "Usage: SCENE.obj OUTPUT.bmp [--normals] [--distances]");

    struct scene scene;
    scene_init(&scene);

    // initialize the frame buffer (the buffer that will store the result of the
    // rendering)
    struct rgb_image *image = rgb_image_alloc(500, 500);

    // set all the pixels of the image to black
    struct rgb_pixel bg_color = {0};
    rgb_image_clear(image, &bg_color);

    double aspect_ratio = (double)image->width / image->height;

    // build the scene
    if(!strcmp(argv[1], "test.obj"))
    {
        build_test_scene(&scene, aspect_ratio);
    }
    else
    {
        build_obj_scene(&scene, aspect_ratio);

        if (load_obj(&scene, argv[1]))
            return 41;

    }

    // parse options
    int th = 0;
    int bench = 0;
    render_mode_f renderer = render_shaded;
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "--normals") == 0)
            renderer = render_normals;
        else if (strcmp(argv[i], "--distances") == 0)
            renderer = render_distances;
        else if (strcmp(argv[i], "--procedural") == 0)
            renderer = render_procedural;
        else if(strcmp(argv[i], "--th") == 0)
            th = 1;
        else if(strcmp(argv[i], "--bench") == 0)
            bench = 1;

    }
    clock_t begin = clock();
    if(bench)
    {
        render_all_pixel(renderer, image, &scene,0);
        printf("No thread:\n");
        time_end(begin);
        begin = clock();
        printf("With thread:\n");
        render_all_pixel(renderer, image, &scene,1);
        time_end(begin);

    }
    else
    {
        render_all_pixel(renderer, image, &scene,th);
        time_end(begin);
    }

   // write the rendered image to a bmp file
    FILE *fp = fopen(argv[2], "w");
    if (fp == NULL)
        err(1, "failed to open the output file");

    rc = bmp_write(image, ppm_from_ppi(80), fp);
    fclose(fp);

    // release resources
    scene_destroy(&scene);
    free(image);
    return rc;
}

