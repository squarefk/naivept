#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec.h"
#include "ray.h"
#include <map>
#include <vector>
#include <string>
#include <utility>

class Texture {
	std::map< std::string, std::pair< std::pair<unsigned char*,int>,std::pair<int,int> > > storage;
	int width;
	int height;
	int channels;
	unsigned char* data;
	std::vector<real> photon_p;
	std::vector<Vec> photon_pos,photon_dir,photon_color;
public:
	Texture(std::string file_name);
	Vec get_color(real u, real v);
	Vec get_color(Vec dir);
	void generate_photon(Ray& photon, Vec& color, real rand_num);
};

#endif