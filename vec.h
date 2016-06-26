#ifndef VEC_H
#define VEC_H

#include "constants.h"
#include <cmath>

class Vec {
public:
	real x, y, z;
	Vec(real _x = 0.0, real _y = 0.0, real _z = 0.0) {
		x = _x;
		y = _y;
		z = _z;
	}
	inline real length() const;
	inline Vec normal() const;
	inline Vec blend(const Vec& rhs) const;
	inline Vec operator + (const Vec& rhs) const;
	inline Vec operator - () const;
	inline Vec operator - (const Vec& rhs) const;
	inline real operator * (const Vec& rhs) const;
	inline Vec operator * (const real rhs) const;
	inline Vec operator % (const Vec& rhs) const;
};

inline real Vec::length() const {
		return sqrt(x * x + y * y + z * z);
}

inline Vec Vec::normal() const {
	real _length = length();
	return Vec(x / _length, y / _length, z / _length);
}

inline Vec Vec::blend(const Vec& rhs) const {
	return Vec(x * rhs.x, y * rhs.y, z * rhs.z);
}

inline Vec Vec::operator + (const Vec& rhs) const {
	return Vec(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vec Vec::operator - () const {
	return Vec(-x, -y, -z);
}

inline Vec Vec::operator - (const Vec& rhs) const {
	return Vec(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline real Vec::operator * (const Vec& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

inline Vec Vec::operator * (const real rhs) const {
	return Vec(x * rhs, y * rhs, z * rhs);
}

inline Vec Vec::operator % (const Vec& rhs) const {
	return Vec(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

#endif 