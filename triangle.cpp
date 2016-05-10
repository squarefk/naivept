#include "triangle.h"

Triangle::Triangle(Vec _p, Vec _q, Vec _r, Vec _c, Material _m) {
	p = _p;
	q = _q;
	r = _r;
	n = ((p - r) % (q - r)).normal();
	np = nq = nr = n;
	c = _c;
	m = _m;
}
