#ifndef GRID_H
#define GRID_H

#include "hpoint.h"
#include "vec.h"
#include "constants.h"

#include <vector>

class Grid {
	std::vector<HPoint> points;
	std::vector<HPoint*>* hash_table;
	Vec low, high, ssize;
	float hash_s;
	int hash_size;
	unsigned int hash_f(int x,int y,int z);
	float pic_float[HEIGHT][WIDTH][3];
	unsigned char pic_unsigned_char[HEIGHT][WIDTH][3];

public:
	Grid(std::vector<HPoint>& points, int h, int w);
	std::vector<HPoint*>& find_possible_near_points(Vec x);
	unsigned char to_unsigned_char(float x);
	void output_picture(int photon_number);
};

#endif