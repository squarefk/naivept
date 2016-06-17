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
	real hash_s;
	int hash_size;
	unsigned int hash_f(int x,int y,int z);
	real pic_real[HEIGHT][WIDTH][3];
	real old_real[HEIGHT][WIDTH][3];
	unsigned char pic_unsigned_char[HEIGHT][WIDTH][3];

public:
	Grid();
	void build(std::vector<HPoint>& _points, int h, int w, int epochs_number, real& global_r2);
	std::vector<HPoint*>& find_possible_near_points(Vec x);
	unsigned char to_unsigned_char(real x);
	void output_picture(int photon_number, int epochs_number, real global_r2);
	void save_picture(int photon_number, real global_r2);
};

#endif