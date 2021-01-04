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
#include "function_procedural.h"
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
/**
**  Print the render options and the options. Print the noise options only if
**  usefull (--procedural[1-3])
**/
void print_options(struct scene *scene, char render_options[][30],
                   char noise_options[][30], int render_index, int noise_index)
{
    // print options
    printf("\tRAYTRACING OPTIONS\n");
    printf("render type: %s\n", render_options[render_index]);
    if (render_index >= 3)
    {
        printf("noise type: %s\n", noise_options[noise_index]);
        printf("absolute noise: ");
        scene->abs ? printf("Yes\n") : printf("No\n");
    }
    printf("multi-threading: ");
    scene->threading ? printf("Yes\n") : printf("No\n");
    printf("anti-aliasing: ");
    scene->anti_aliasing ? printf("Yes\n") : printf("No\n");
}
/**
**  Parse the options set the scene options variable and return the render_mode
** option
**/
render_mode_f parse_options(struct scene *scene, int argc, char *argv[])
{
    // parse options
    render_mode_f renderer = render_shaded;
    scene->noise_function = fbm3d;
    scene->abs = 0;

    char noise_options[8][30]
        = {"--fbm",    "--warping",          "--interference", "--ribbon",
           "--galaxy", "--galaxy-distorted", "--pastel",       "--hypnotic"};
    noise_f *noise_arr[8] = {fbm3d,  domain_warping3d, interference, ribbon,
                             galaxy, galaxy_distorted, pastel,       hypnotic};
    int noise_index = 0;

    char render_options[6][30]
        = {"--shaded",      "--normals",     "--distances",
           "--procedural1", "--procedural2", "--procedural3"};
    render_mode_f render_arr[6]
        = {render_shaded,      render_normals,     render_distances,
           render_procedural1, render_procedural2, render_procedural3};
    int render_index = 0;

    for (int i = 3; i < argc; i++)
    {
        for (int j = 0; j < 6; j++)
            if ((strcmp(argv[i], render_options[j]) == 0))
            {
                render_index = j;
                renderer = render_arr[j];
            }
        for (int j = 0; j < 8; j++)
        {
            if ((strcmp(argv[i], noise_options[j]) == 0))
            {
                noise_index = j;
                scene->noise_function = noise_arr[j];
            }
        }

        if ((strcmp(argv[i], "--abs") == 0))
            scene->abs = 1;
        else if ((strcmp(argv[i], "--th") == 0)
                 || (strcmp(argv[i], "--multi-threading") == 0))
            scene->threading = 1;
        else if ((strcmp(argv[i], "--anti-aliasing") == 0)
                 || (strcmp(argv[i], "--AA") == 0))
            scene->anti_aliasing = 1;
    }

    print_options(scene, render_options, noise_options, render_index,
                  noise_index);
    return renderer;
}
int main(int argc, char *argv[])
{
    int rc;

    if (argc < 3)
    {
        errx(1, "Usage: SCENE.obj OUTPUT.bmp [RENDER_OPTIONS] [NOISE_OPTIONS] "
                "[OPTIONS]\n\n"
                "RENDER_OPTIONS:\n"
                "--shaded --normals --distances --procedural1 --procedural2 "
                "--procedural3\n\n"
                "NOISE_OPTIONS: (usefull only when using --procedural[1-3])\n"
                "--fbm --warping --interference --ribbon"
                "--galaxy --galaxy-distorted --pastel --hypnotic\n\n"
                "OPTIONS:\n"
                "--abs (absolute value of a noise usefull only when using\n"
                "--procedural[1-3])\n"
                "--multi-threading | --th\n"
                "--anti-aliasing | --AA");
    }

    struct scene scene;
    scene_init(&scene);

    // initialize the frame buffer (the buffer that will store the result of the
    // rendering)
    struct rgb_image *image = rgb_image_alloc(200, 200);

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
    render_mode_f renderer = parse_options(&scene, argc, argv);
    render_all_pixel(renderer, image, &scene);

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
