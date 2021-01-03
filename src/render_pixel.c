#include "render_pixel.h"

// number of thread
#define NUMTH 16
/**
** Join all the thread of an array, up to the number siz
**/
void join_all(pthread_t *th, int size)
{
    for (int j = 0; j < size; j++)
        pthread_join(th[j], NULL);
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
    // printf("rendering: %li/%li\n", starty, maxx);
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
    // printf("rendering: %li/%li\n", starty, maxx);
}

void render_image(void *rend_void)
{
    struct render_struct *rend = rend_void;
    // render all pixels
    size_t maxy = rend->maxy;
    int i = 0;
    for (size_t y = 0; y < maxy; y++)
    {
        render_line(rend_void);
        rend->starty++;
        i++;
    }
}

void render_image_th(void *rend_void)
{
    struct render_struct *rend = rend_void;
    struct rgb_image *image = rend->image;
    struct scene *scene = rend->scene;
    render_mode_f renderer = rend->renderer;
    size_t maxy = rend->maxy;
    free(rend);
    rend_void = alloc_render_struct(renderer, image, scene, 0, 0, image->height,
                                    image->width);
    // render all pixels
    pthread_t th[NUMTH];
    int i = 0;
    for (size_t y = 0; y < maxy; y++)
    {
        pthread_create(&th[i], NULL, render_line_th, rend_void);
        if (y != maxy - 1)
            rend_void = alloc_render_struct(renderer, image, scene, 0, y,
                                            image->height, image->width);
        // printf("%li\n",y);
        i++;
        if (i > NUMTH)
        {
            join_all(th, NUMTH);
            i = 0;
        }
    }
    join_all(th, i);
}
void render_all_pixel(render_mode_f renderer, struct rgb_image *image,
                      struct scene *scene)
{
    struct render_struct *rend = 0;
    rend = alloc_render_struct(renderer, image, scene, 0, 0, image->height,
                               image->width);
    if (scene->threading)
        render_image_th(rend);
    else
        render_image(rend);
}
