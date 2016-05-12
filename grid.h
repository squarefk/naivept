#ifndef GRID_H
#define GRID_H

#include "hpoint.h"
#include "vec.h"
#include "constants.h"

#include <vector>

class Grid {
	std::vector<HPoint>* hash_table;
	Vec low, high, ssize;
	float hash_s;
	int hash_size;
	unsigned int hash_f(int x,int y,int z);
	Vec pic[HEIGHT][WIDTH];

public:
	Grid(std::vector<HPoint>& points, int h, int w);
	std::vector<HPoint>& find_possible_near_points(Vec x);
	int to_int(float x);
	void output_picture(int photon_number);
};

#endif