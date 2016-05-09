#ifndef VEC_H
#define VEC_H

class Vec {
public:
	float x, y, z;
	Vec(float _x = 0.0, float _y = 0.0, float _z = 0.0);
	float length() const;
	Vec normal() const;
	Vec blend(const Vec& rhs) const;
	Vec operator + (const Vec& rhs) const;
	Vec operator - () const;
	Vec operator - (const Vec& rhs) const;
	float operator * (const Vec& rhs) const;
	Vec operator * (const float rhs) const;
	Vec operator % (const Vec& rhs) const;
};

#endif