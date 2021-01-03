______  _____   _____________  ___  _____ _____ _   _ _____
| ___ \/ _ \ \ / /_   _| ___ \/ _ \/  __ \_   _| \ | |  __ \
| |_/ / /_\ \ V /  | | | |_/ / /_\ \ /  \/ | | |  \| | |  \/
|    /|  _  |\ /   | | |    /|  _  | |     | | | . ` | | __
| |\ \| | | || |   | | | |\ \| | | | \__/\_| |_| |\  | |_\ \
\_| \_\_| |_/\_/   \_/ \_| \_\_| |_/\____/\___/\_| \_/\____/


Made by: alexandre.dias

Feature:

    Multi-threading

    Anti-Aliasing

    Procedural Textures


Options:

    --normals (render the normals of the object)

    --distances (render the image in grayscale based on the
    distances to the camera)

    --perlin1 (render the object using a perlin noise taking in consideration
    only the position of a pixel)

    --perlin2 (render the object using an absolute perlin noise)

    --perlin3 (render the object using an absolute perlin noise for each color)

    --th | --multi-threading (Enable multi threading)

    --anti-aliasing | --AA (Enable anti aliasing)

Multi-threading: --th | --multi-threading

    When activated, instead of rendering pixel per pixel, send a line to be
    rendered with pthread_create, once the maximum number of thread chosen
    has been atteined, pthread_join all of them and repeat until all the lines
    are drawn

    render_pixel.c:
        contains the functions that render the image,
        with their multi-threaded variant

    render_struct.h:
        contains the definition of the struct passed to
        pthread_create

Anti-aliasing: --anti-aliasing | --AA

    When activated, instead of sending only one ray per pixel, 4 rays are sent
    The color is averaged from those 4 rays

    pixel_color.c:
        contains the function calling the 4 rays and averaging the colors of
        pixels

    render_mode.c:
        contains the different functions for rendering the rays (distances,
        normals, perlin ...)


Procedural Textures: --perlin1 --perlin2 --perlin3

    Procedural textures in the form of a perlin noise and using it with
    Fractal Bronian motions (fbm)

