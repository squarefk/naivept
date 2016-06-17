#include "sphere.h"
	
Sphere::Sphere(real _r, Vec _p, Vec _l, Vec _c, Material _m) {
	radius = _r;
	pos = _p;
	light = _l;
	color = _c;
	material = _m;
}
