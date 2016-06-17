#include "ray.h"

#include <cmath>
#include <cstdio>

Ray::Ray(Vec _p, Vec _d) {
	pos = _p;
	dir = _d;
	// equal to zero if new ray
	if (fabs(_d.length()-1.0)>1e-4 && _d.length()>1e-4) {
		fprintf(stderr, "Error on the direction of the ray.\n");
	}
	inv = Vec(1.0/dir.x, 1.0/dir.y, 1.0/dir.z);
}

bool Ray::intersect_with_sphere(const Sphere& sphere, real& dist) const {
	Vec _x = dir;
	Vec _y = sphere.pos - pos;
	real b = _x * _y;
	real c = _y * _y - sphere.radius * sphere.radius;
	real delta = b * b - c;
	if (delta < 0) return false;
	delta = sqrt(delta);
	real eps = 1e-4f;
	if (b - delta > eps) {
		dist = b - delta;
		return true;
	} else
	if (b + delta > eps) {
		dist = b + delta;
		return true;
	} else
		return false;
}

bool Ray::intersect_with_triangle(const Triangle& tri, real& dist, Vec& norm) const {
	real t,u,v;
	Vec E1 = tri.q - tri.p;
	Vec E2 = tri.r - tri.p;
	Vec P = dir % E2;
	real det = E1 * P;
	Vec T;
	if (det > 0) {
		T = pos - tri.p;
	} else {
		T = tri.p - pos;
		det = -det;
	}
	static real eps = 1e-6;
	if (det < eps) return false;
	u = T * P;
	if (u < -eps || u > det + eps) return false;
	Vec Q = T % E1;
	v = dir * Q;
	if (v < -eps || u + v > det + eps) return false;
	t = E2 * Q;

	real fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;

	dist = t;
	norm = (tri.np * (1.0 - u - v) + tri.nq * u + tri.nr * v).normal();

	return true;
}

// modify tmin > tmax
bool Ray::intersect_with_box(const Vec& low, const Vec& high) const {
	real tmin=-1e30,tmax=1e30;
	real _tmin,_tmax;
	bool sign;
#define TEST(t)\
	if (fabs(dir.t)>1e-6)\
	{\
		sign=(inv.t<0);\
		_tmin=((sign?high.t:low.t)-pos.t)*inv.t;\
		_tmax=((sign?low.t:high.t)-pos.t)*inv.t;\
		if (_tmin > tmin) tmin=_tmin;\
		if (_tmax < tmax) tmax=_tmax;\
		if (tmin > tmax + 1e-6) return false;\
	}
	TEST(x); TEST(y); TEST(z);
#undef TEST
	return true;
}
