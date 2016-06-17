#ifndef SPHERE_H
#define SPHERE_H

#include "vec.h"
#include "constants.h"

struct Sphere {
	real radius;
	Vec pos, light, color;
	Material material;
	Sphere(real _r, Vec _p, Vec _l, Vec _c, Material _m);
};


#endif