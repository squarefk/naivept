#ifndef SPHERE_H
#define SPHERE_H

#include "vec.h"
#include "constants.h"

struct Sphere {
	float radius;
	Vec pos, light, color;
	Material material;
	Sphere(float _r, Vec _p, Vec _l, Vec _c, Material _m);
};


#endif