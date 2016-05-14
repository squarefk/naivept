#include "grid.h"

#include <cmath>
#include <string>
#include <cstring>
#include <algorithm>
using namespace std;

Grid::Grid(vector<HPoint>& _points, int h, int w) {
	fprintf(stderr, "Start grid generation.\n");

	// copy _points to the inside vector
	for (int i = 0; i < _points.size(); ++i) {
		points.push_back(_points[i]);
	}

	low = high = points[0].x;
	for (int i = 0; i < points.size(); ++i) {
		low.x=min(low.x,points[i].x.x);
		low.y=min(low.y,points[i].x.y);
		low.z=min(low.z,points[i].x.z);
		high.x=max(high.x,points[i].x.x);
		high.y=max(high.y,points[i].x.y);
		high.z=max(high.z,points[i].x.z);
	}
	Vec ssize = high-low;
	float irad = ((ssize.x + ssize.y + ssize.z) / 3.0) / ((w + h) / 2.0) * 2.0;
	low = low - Vec(1.0,1.0,1.0) * irad;
	high = high + Vec(1.0,1.0,1.0) * irad;

	hash_s = 1.0 / (irad * 2.0);
	hash_size = points.size();
	hash_table = new vector<HPoint*>[hash_size];

	for (int i = 0; i < points.size(); ++i) {
		points[i].r2 = irad * irad;
		points[i].cnt = 0;
		points[i].flux = Vec();
		Vec corner = (points[i].x - Vec(1.0,1.0,1.0) * irad - low) * hash_s;
		for (int dx = 0; dx < 2; ++dx)
			for (int dy = 0; dy < 2; ++dy)
				for (int dz = 0; dz < 2; ++dz)
					hash_table[hash_f(int(corner.x)+dx,int(corner.y)+dy,int(corner.z)+dz)].push_back(&points[i]);
	}
	fprintf(stderr, "Finish grid generation.\n");
}

unsigned int Grid::hash_f(int x,int y,int z) {
	return (unsigned int)((x*73856093)^(y*19349663)^(z*83492791))%hash_size;
}

vector<HPoint*>& Grid::find_possible_near_points(Vec x) {
//	fprintf(stderr,"(%.2f %.2f %.2f)  (%.2f %.2f %.2f)  (%.2f %.2f %.2f)   %d\n", low.x,low.y,low.z, high.x,high.y,high.z, x.x,x.y,x.z, hash_table[hash_f(int(x.x),int(x.y),int(x.z))].size());
	x = (x - low) * hash_s;
	return hash_table[hash_f(int(x.x),int(x.y),int(x.z))];
}

int Grid::to_int(float x) {
	return int(pow(1-exp(-x),1/2.2)*255+0.5);
}

void Grid::output_picture(int photon_number) {
	// clear picture
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			pic[i][j] = Vec();
	// calc the whole picture
	for (int i = 0; i < points.size(); ++i) {
		const HPoint& point = points[i];
		pic[point.h][point.w] = pic[point.h][point.w] + point.flux * (1.0 / (M_PI * point.r2 * photon_number));
	}
	// output to the file
	string file_name = string("output/image") + to_string(photon_number/10000) + string("w.ppm");
	FILE* file = fopen(file_name.c_str(), "w");
	fprintf(file, "P3\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
	for (int h = 0; h < HEIGHT; ++h)
		for (int w = 0; w < WIDTH; ++w) {
			int r = to_int(pic[h][w].x);
			int g = to_int(pic[h][w].y);
			int b = to_int(pic[h][w].z);
			fprintf(file, "%d %d %d ", r, g, b);
		}
}
