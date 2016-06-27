#include "grid.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tools/stb_image_write.h"

#include <cmath>
#include <string>
#include <cstring>
#include <algorithm>
using namespace std;

Grid::Grid() {
	memset(old_real,0,sizeof(old_real));
}

void Grid::build(vector<HPoint>& _points, int h, int w, int epochs_number, real& global_r2) {
	points.clear();
	if (hash_table != NULL) {
		delete [] hash_table;
	}

	// copy _points to the inside vector
	for (int i = 0; i < _points.size(); ++i) {
		if ((i+1)%(_points.size()/10000)==_points.size()%(_points.size()/10000))
			fprintf(stderr, "\rGrid hash generating %.2f%%", 1.0*(i+1)/_points.size()*100.0*0.1);
		points.push_back(_points[i]);
	}

	low = high = points[0].x;
	for (int i = 0; i < points.size(); ++i) {
		if ((i+1)%(points.size()/10000)==points.size()%(points.size()/10000))
			fprintf(stderr, "\rGrid hash generating %.2f%%", 10.0+1.0*(i+1)/points.size()*100.0*0.1);
		if (!points[i].collect)
			continue;
		low.x=min(low.x,points[i].x.x);
		low.y=min(low.y,points[i].x.y);
		low.z=min(low.z,points[i].x.z);
		high.x=max(high.x,points[i].x.x);
		high.y=max(high.y,points[i].x.y);
		high.z=max(high.z,points[i].x.z);
	}
	Vec ssize = high-low;
	real irad = ((ssize.x + ssize.y + ssize.z) / 3.0) / ((w + h) / 2.0) * 2.0;
	low = low - Vec(1.0,1.0,1.0) * irad;
	high = high + Vec(1.0,1.0,1.0) * irad;
	if (epochs_number == 1) {
		global_r2 = irad * irad * INIT_GLOBAL_RATE;
	} else {
		global_r2 *= (1.0 * (epochs_number - 2.0) + ALPHA) / (1.0 * (epochs_number - 2.0) + 1.0);
	}

	hash_s = 1.0 / (irad * 2.0);
	hash_size = points.size();
	hash_table = new vector<HPoint*>[hash_size];

	for (int i = 0; i < points.size(); ++i) {
		if ((i+1)%(points.size()/10000)==points.size()%(points.size()/10000))
			fprintf(stderr, "\rGrid hash generating %.2f%%", 20.0+1.0*(i+1)/points.size()*100.0*0.8);
		if (!points[i].collect)
			continue;
		points[i].cnt = 0;
		points[i].flux = Vec();
		Vec corner = (points[i].x - Vec(1.0,1.0,1.0) * irad - low) * hash_s;
		for (int dx = 0; dx < 2; ++dx)
			for (int dy = 0; dy < 2; ++dy)
				for (int dz = 0; dz < 2; ++dz)
					hash_table[hash_f(int(corner.x)+dx,int(corner.y)+dy,int(corner.z)+dz)].push_back(&points[i]);
	}
	fprintf(stderr, "\n");
}

unsigned int Grid::hash_f(int x,int y,int z) {
	return (unsigned int)((x*73856093)^(y*19349663)^(z*83492791))%hash_size;
}

vector<HPoint*>& Grid::find_possible_near_points(Vec x) {
//	fprintf(stderr,"(%.2f %.2f %.2f)  (%.2f %.2f %.2f)  (%.2f %.2f %.2f)   %d\n", low.x,low.y,low.z, high.x,high.y,high.z, x.x,x.y,x.z, hash_table[hash_f(int(x.x),int(x.y),int(x.z))].size());
	x = (x - low) * hash_s;
	return hash_table[hash_f(int(x.x),int(x.y),int(x.z))];
}

real Grid::calc_prop(int epochs_number) {
	real ret=0;
	real rio[]={0.299,0.587,0.114};
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j)
			for (int k = 0; k < 3; ++k)
				ret += (pic_real[i][j][k] + old_real[i][j][k]) / epochs_number * rio[k];
	return EXPOSURE / (ret / HEIGHT / WIDTH);
}

real Grid::each_lum(real r, real g, real b, real prop) {
	real bef = (0.299 * r + 0.587 * g + 0.114 * b) * prop;
	return 1 / (1 + bef);
}

unsigned char Grid::to_unsigned_char(real x, real prop) {
	x *= prop;
	return (unsigned char)(pow(x,1/2.2) * 255 + 0.5);
}

void Grid::output_picture(int photon_number, int epochs_number, real global_r2) {
//	fprintf(stderr, "%d\n",photon_number);
//	fprintf(stderr, "%.10f\n",global_r2);

	// clear picture
	memset(pic_real,0,sizeof(pic_real));
	memset(pic_unsigned_char,0,sizeof(pic_unsigned_char));

	// calc the whole picture
	for (int i = 0; i < points.size(); ++i) {
		const HPoint& point = points[i];
		pic_real[point.h][point.w][0] += point.flux.x * (1.0 / (M_PI * global_r2 * photon_number));
		pic_real[point.h][point.w][1] += point.flux.y * (1.0 / (M_PI * global_r2 * photon_number));
		pic_real[point.h][point.w][2] += point.flux.z * (1.0 / (M_PI * global_r2 * photon_number));
	}

	real prop = calc_prop(epochs_number);

	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j) {
			double lum_r = each_lum((pic_real[i][j][0] + old_real[i][j][0]) / epochs_number,
									(pic_real[i][j][1] + old_real[i][j][1]) / epochs_number,
									(pic_real[i][j][2] + old_real[i][j][2]) / epochs_number, prop);
			for (int k = 0; k < 3; ++k)
				pic_unsigned_char[i][j][k] = to_unsigned_char((pic_real[i][j][k] + old_real[i][j][k]) / epochs_number, prop * lum_r);
		}

	// output to the file
	string file_name = string("output/image_") + to_string(epochs_number) + string("_") + to_string(photon_number/10000) + string("w.png");
	if (!stbi_write_png(file_name.c_str(), WIDTH, HEIGHT, 3, pic_unsigned_char, WIDTH*3)) {
		fprintf(stderr, "Occur error when outputing to picture\n");
	}
}

void Grid::save_picture(int photon_number, real global_r2) {
	// calc the whole picture
	for (int i = 0; i < points.size(); ++i) {
		const HPoint& point = points[i];
		old_real[point.h][point.w][0] += point.flux.x * (1.0 / (M_PI * global_r2 * photon_number));
		old_real[point.h][point.w][1] += point.flux.y * (1.0 / (M_PI * global_r2 * photon_number));
		old_real[point.h][point.w][2] += point.flux.z * (1.0 / (M_PI * global_r2 * photon_number));
	}
}

#undef STB_IMAGE_WRITE_IMPLEMENTATION
