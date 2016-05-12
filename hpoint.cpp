#include "hpoint.h"

HPoint::HPoint(Vec _x, Vec _n, Vec _c, int _h, int _w) {
	x = _x;
	n = _n;
	c = _c;
	h = _h;
	w = _w;
}

HPoint::HPoint(const HPoint& p) {
	x = p.x;
	n = p.n;
	c = p.c;
	h = p.h;
	w = p.w;
	r2 = p.r2;
	cnt = p.cnt;
	flux = p.flux;
}