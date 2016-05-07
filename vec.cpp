#include "vec.h"

#include <cmath>

Vec::Vec(double _x, double _y, double _z) {
	x = _x;
	y = _y;
	z = _z;
}

double Vec::length() const {
		return sqrt(x * x + y * y + z * z);
}

Vec Vec::normal() const {
	double _length = length();
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

double Vec::operator * (const Vec& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vec Vec::operator * (const double rhs) const {
	return Vec(x * rhs, y * rhs, z * rhs);
}

Vec Vec::operator % (const Vec& rhs) const {
	return Vec(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}