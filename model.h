#ifndef MODEL_H
#define MODEL_H

#include "triangle.h"
#include "ray.h"
#include "constants.h"
#include <vector>
using namespace std;

struct Node {
	int l, r, m;
	Vec low;
	Vec high;
};

class Model {
	vector<Triangle> triangles;

	Node* t;
	int* queue;

public:
	void load_from_obj(const char* file_name);

	bool intersect(const Ray& ray, real& dist, Vec& n, Vec& c, Material& m);
	void build(int k, int l, int r);
};

#endif