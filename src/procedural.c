#include "procedural.h"
#include <stdlib.h>
#include "perlin.h"



struct vec3 procedural_texture(struct vec3 point, struct vec3 normal)
{
    struct vec3 res;
    res.x =Perlin_Get2d(point.x, normal.x,0.8,3.0);
    res.y = Perlin_Get2d(point.y, normal.y,0.8, 3.0);
    res.z =Perlin_Get2d(point.z, normal.z,0.8, 3.0);
    return res;
}

struct vec3 perlin_texture(struct vec3 point, struct vec3 normal)
{
    struct vec3 res;
    double noise1 = noise3(point.x, point.y,point.z);
    double noise2 = noise3(point.y, point.z,point.x);
    double noise4 = noise3(point.z, point.x,point.y);
    res.x =noise1;
    res.y = noise2;
    res.z =noise4;
    return res;
}
double fbm(struct vec3 x, double H)
{    
    float t = 0.0;
    for( int i=0; i<7; i++ )
    {
        float f = pow( 2.0, i );
        float a = pow( f, -H );
        t += a*noise2d(f,a);
    }
    printf("%f\n",t/355);
    return t/455;
}
struct vec3 fbm_texture(struct vec3 point, struct vec3 normal)
{
    struct vec3 res;
    res.x =fbm(point, 1);
    res.y =fbm(point,1);
    res.z =fbm(point,1);
    return res;
}

struct vec3 procedural_shader(const struct material *base_material,
                          const struct intersection *inter,
                          const struct scene *scene, const struct ray *ray)
{
    (void)base_material;
    (void)scene;
    (void)ray;
    return perlin_texture(inter->point, inter->normal);
}




