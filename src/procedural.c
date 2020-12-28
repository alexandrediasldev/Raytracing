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
    //size of the pattern
    double freq = 9.0;
    //bluriness of the pattern
    double depth = 5.0;
    struct vec3 res;
    double noise1 = noise3d(point.x, point.y,point.z,freq,depth);
    if(noise1> 1.0)
    printf("%f\n",noise1);
    res.x =noise1;
    res.y = noise1;
    res.z =noise1;
    return res;
}
struct vec3 perlin_texture2(struct vec3 point, struct vec3 normal)
{
    struct vec3 res;
    double a = 0.5;
    double b = 9.0;
    double noise1 = Perlin_Get3d(point.x, point.y,point.z,a,b);
    double noise2 = Perlin_Get3d(normal.x, normal.y,normal.z,a,b);
    if(noise1 > 1.0 && noise2 > 1.0)
        printf("%f %f\n",noise1, noise2);
    res.x =(noise1+noise2)/2.0;
    res.y = (noise1+noise2)/2.0;
    res.z =(noise1+noise2)/2.0;
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




