#include "vec.h"

#include <cmath>

Vec::Vec(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

float Vec::length() const {
		return sqrt(x * x + y * y + z * z);
}

Vec Vec::normal() const {
	float _length = length();
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

float Vec::operator * (const Vec& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vec Vec::operator * (const float rhs) const {
	return Vec(x * rhs, y * rhs, z * rhs);
}

Vec Vec::operator % (const Vec& rhs) const {
	return Vec(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}