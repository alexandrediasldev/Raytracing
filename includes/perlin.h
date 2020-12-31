#pragma once
#include <math.h>

double Perlin_Get2d(double x, double y, double freq, int depth);
double Perlin_Get3d(double x, double y, double z, double freq, int depth);
double noise2d(double x, double y);
double noise3(double x, double y, double z);
double noise3d(double x, double y, double z, double freq, int depth);
void init_perlin(void);
