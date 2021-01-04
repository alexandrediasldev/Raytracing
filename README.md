______  _____   _____________  ___  _____ _____ _   _ _____
| ___ \/ _ \ \ / /_   _| ___ \/ _ \/  __ \_   _| \ | |  __ \
| |_/ / /_\ \ V /  | | | |_/ / /_\ \ /  \/ | | |  \| | |  \/
|    /|  _  |\ /   | | |    /|  _  | |     | | | . ` | | __
| |\ \| | | || |   | | | |\ \| | | | \__/\_| |_| |\  | |_\ \
\_| \_\_| |_/\_/   \_/ \_| \_\_| |_/\____/\___/\_| \_/\____/


Made by: alexandre.dias

Usage: SCENE.obj OUTPUT.bmp [RENDER_OPTIONS] [NOISE_OPTIONS] [OPTIONS]

Features:

    Multi-threading

    Anti-Aliasing

    Procedural Textures


Render Options:


    --shaded (render the object with it's material colors)

    --normals (render the normals of the object)

    --distances (render the image in grayscale based on the distances to the
    camera)

    --procedural1 (render a procedural texture based only on coordinates
    and discarding the normals. The image is in grayscale)


    --procedural2 (render a procedural texture based on coordinates
    and normals. The image is in grayscale)

    --procedural3 (render a procedural texture based on coordinates
    and normals. The image is colored)


    default value: --shaded

Noise Options: (usefull only when using --procedural[1-3])

    Classic : (well known fonctions)

    --fbm (fractal brownian motion)

    --warping (distorted fractal brownina motion)

    --interference (fractal brownian motion with absolute value
    to produce lines on the pattern)

    Custom : (made be me using fbm and warping)

    --ribbon

    --galaxy

    --galaxy-distorted

    --pastel

    --hypnotic

    default value: --fbm

Options:

    --th | --multi-threading (Enable multi threading)
    default value: off

    --anti-aliasing | --AA (Enable anti aliasing)
    default value: off

    --abs
    default value: off

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


Procedural Textures: --procedural[1-3] and all the Noise options

    Procedural textures using a perlin noise with a function to alter
    the noise.

    perlin.c:
        contains the functions creating the perlin noise.

    function_procedural.c:
        contains the functions altering the noise.

    procedural.c:
        contains the functions for the different procedural
        patterns (--procedural[1-3])



