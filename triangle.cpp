#include "triangle.h"

Triangle::Triangle(Vec _p, Vec _q, Vec _r) {
	p = _p;
	q = _q;
	r = _r;
	n = ((p - r) % (q - r)).normal();
}
