#include "ray.h"

#include <cmath>

Ray::Ray(Vec _p, Vec _d) {
	pos = _p;
	dir = _d;
}

bool Ray::intersect_with_sphere(const Sphere& sphere, double& dist) const {
	Vec _x = dir;
	Vec _y = sphere.pos - pos;
	double b = _x * _y;
	double _y_length = _y.length();
	double c = _y_length * _y_length - sphere.radius * sphere.radius;
	double delta = b * b - c;
	if (delta < 0) return false;
	delta = sqrt(delta);
	double eps = 1e-4;
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

bool Ray::intersect_with_triangle(const Triangle& tri, double& dist) const {
	double t,u,v;
	Vec E1 = tri.q - tri.p;
	Vec E2 = tri.r - tri.p;
	Vec P = dir % E2;
	double det = E1 * P;
	Vec T;
	if (det > 0) {
		T = pos - tri.p;
	} else {
		T = tri.p - pos;
		det = -det;
	}
	if (det < 1e-4) return false;
	u = T * P;
	if (u<0 || u>det) return false;
	Vec Q = T % E1;
	v = dir * Q;
	if (v<0 || u+v>det) return false;
	t = E2 * Q;

	dist = t / det;
	return true;

	double fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;
	return true;
}

bool Ray::intersect_with_box(const Vec& low, const Vec& high) const {
	Vec inv = Vec(1.0/dir.x, 1.0/dir.y, 1.0/dir.z);
	double tmin=-1e30,tmax=1e30;
	double _tmin,_tmax;
	bool sign;
#define TEST(t)\
	if (fabs(dir.t)>1e-4)\
	{\
		sign=(inv.t<0);\
		_tmin=((sign?high.t:low.t)-pos.t)*inv.t;\
		_tmax=((sign?low.t:high.t)-pos.t)*inv.t;\
		if (_tmin > tmin) tmin=_tmin;\
		if (_tmax < tmax) tmax=_tmax;\
		if (tmin+1e-4 > tmax) return false;\
	}
	TEST(x); TEST(y); TEST(z);
#undef TEST
	return tmax > 1e-4;
}
