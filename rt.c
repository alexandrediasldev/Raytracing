#include <err.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bmp.h"
#include "camera.h"
#include "image.h"
#include "normal_material.h"
#include "obj_loader.h"
#include "phong_material.h"
#include "procedural.h"
#include "scene.h"
#include "sphere.h"
#include "triangle.h"
#include "vec3.h"

#include "perlin.h"
#include "render_mode.h"
#include "render_pixel.h"
#include "render_struct.h"
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
    if (!strcmp(argv[1], "test.obj"))
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
    render_mode_f renderer = render_shaded;
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "--normals") == 0)
            renderer = render_normals;
        else if (strcmp(argv[i], "--distances") == 0)
            renderer = render_distances;
        else if (strcmp(argv[i], "--perlin1") == 0)
            renderer = render_perlin1;
        else if (strcmp(argv[i], "--perlin2") == 0)
            renderer = render_perlin2;
        else if (strcmp(argv[i], "--perlin3") == 0)
            renderer = render_perlin3;
        else if (strcmp(argv[i], "--th") == 0)
            th = 1;
    }
    render_all_pixel(renderer, image, &scene, th);

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
