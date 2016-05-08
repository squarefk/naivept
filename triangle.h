#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec.h"

class Triangle{
public:
	Vec p, q, r, n;
	Triangle(Vec _p, Vec _q, Vec _r);
};

#endif
