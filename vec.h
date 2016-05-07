#ifndef VEC_H
#define VEC_H

class Vec {
public:
	double x, y, z;
	Vec(double _x = 0.0, double _y = 0.0, double _z = 0.0);
	double length() const;
	Vec normal() const;
	Vec blend(const Vec& rhs) const;
	Vec operator + (const Vec& rhs) const;
	Vec operator - () const;
	Vec operator - (const Vec& rhs) const;
	double operator * (const Vec& rhs) const;
	Vec operator * (const double rhs) const;
	Vec operator % (const Vec& rhs) const;
};

#endif