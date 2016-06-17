#ifndef VEC_H
#define VEC_H

#include "constants.h"

class Vec {
public:
	real x, y, z;
	Vec(real _x = 0.0, real _y = 0.0, real _z = 0.0);
	real length() const;
	Vec normal() const;
	Vec blend(const Vec& rhs) const;
	Vec operator + (const Vec& rhs) const;
	Vec operator - () const;
	Vec operator - (const Vec& rhs) const;
	real operator * (const Vec& rhs) const;
	Vec operator * (const real rhs) const;
	Vec operator % (const Vec& rhs) const;
};

#endif