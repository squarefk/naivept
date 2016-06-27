#ifndef HPOINT_H
#define HPOINT_H

#include "vec.h"

class HPoint {
public:
	// information of final image
	int h, w;
	// get from eye ray
	Vec x, n, c;
	unsigned int cnt;
	Vec flux;
	bool collect;

	HPoint(Vec _x, Vec _n, Vec _c, int _h, int _w, Vec _flux = Vec(0,0,0), bool _collect = true);
	HPoint(const HPoint& p);
};

#endif