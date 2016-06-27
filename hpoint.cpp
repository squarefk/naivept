#include "hpoint.h"

HPoint::HPoint(Vec _x, Vec _n, Vec _c, int _h, int _w, Vec _flux, bool _collect) {
	x = _x;
	n = _n;
	c = _c;
	h = _h;
	w = _w;
	flux = _flux;
	collect = _collect;
}

HPoint::HPoint(const HPoint& p) {
	x = p.x;
	n = p.n;
	c = p.c;
	h = p.h;
	w = p.w;
	cnt = p.cnt;
	flux = p.flux;
	collect = p.collect;
}