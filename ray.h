#ifndef RAY_H
#define RAY_H

#include "sphere.h"
#include "triangle.h"

struct Ray {
	Vec pos, dir;
	Ray(Vec _p = Vec(), Vec _d = Vec());
	bool intersect_with_sphere(const Sphere& sphere, double& dist) const;
	bool intersect_with_triangle(const Triangle& tri, double& dist) const;
	bool intersect_with_box(const Vec& low, const Vec& high) const;
};


#endif