#include "vec.h"

#include <cmath>

Vec::Vec(real _x, real _y, real _z) {
	x = _x;
	y = _y;
	z = _z;
}

real Vec::length() const {
		return sqrt(x * x + y * y + z * z);
}

Vec Vec::normal() const {
	real _length = length();
	return Vec(x / _length, y / _length, z / _length);
}

Vec Vec::blend(const Vec& rhs) const {
	return Vec(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vec Vec::operator + (const Vec& rhs) const {
	return Vec(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec Vec::operator - () const {
	return Vec(-x, -y, -z);
}

Vec Vec::operator - (const Vec& rhs) const {
	return Vec(x - rhs.x, y - rhs.y, z - rhs.z);
}

real Vec::operator * (const Vec& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vec Vec::operator * (const real rhs) const {
	return Vec(x * rhs, y * rhs, z * rhs);
}

Vec Vec::operator % (const Vec& rhs) const {
	return Vec(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}