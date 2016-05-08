#ifndef SPHERE_H
#define SPHERE_H

#include "vec.h"

enum Material{DIFF, SPEC, REFR};

struct Sphere {
	double radius;
	Vec pos, light, color;
	Material material;
	Sphere(double _r, Vec _p, Vec _l, Vec _c, Material _m);
};


#endif