#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec.h"
#include <map>
#include <utility>
#include <string>

class Texture {
	std::map< std::string, std::pair< std::pair<unsigned char*,int>,std::pair<int,int> > > storage;
	int width;
	int height;
	int channels;
	unsigned char* data;
public:
	Texture(std::string file_name);
	Vec get_color(real u, real v);
};

#endif