#pragma once
#include <math.h>

double Perlin_Get2d(double x, double y, double freq, int depth);
double noise2d(double x, double y);
float noise3( float x, float y, float z );
void init_perlin(void);
