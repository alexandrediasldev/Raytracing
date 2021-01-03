#pragma once

typedef double(noise_f(double, double, double, double, int));
double fbm3d(double x, double y, double z, double H, int numOctaves);

double interference(double x, double y, double z, double H, int numOctaves);
double domain_warping3d(double x, double y, double z, double H, int numOctaves);

double ribbon(double x, double y, double z, double H, int numOctaves);

double galaxy(double x, double y, double z, double H, int numOctaves);

double galaxy_distorted(double x, double y, double z, double H, int numOctaves);

double pastel(double x, double y, double z, double H, int numOctaves);

double hypnotic(double x, double y, double z, double H, int numOctaves);
