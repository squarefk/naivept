#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec.h"
#include "constants.h"

class Triangle{
public:
	Vec p, q, r, n, c;
	Material m;
	Triangle(Vec _p, Vec _q, Vec _r, Vec _c = Vec(0.5,0.5,0.5), Material _m = DIFF);
};

#endif
