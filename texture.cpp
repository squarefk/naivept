#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "tools/stb_image.h"

using namespace std;

//map< string, pair< pair<unsigned char*,int>,pair<int,int> > > Texture::storage;

Texture::Texture(string file_name) {
	map< string, pair< pair<unsigned char*,int>,pair<int,int> > >::iterator it = storage.find(file_name);
	if (it == storage.end()) {
		data = stbi_load(file_name.c_str(), &width, &height, &channels, 0);
		if (data == NULL) {
			fprintf(stderr, "Occur error when loading texture\n");
		} else {
			fprintf(stderr, "Load image %s successfully\n", file_name.c_str());
		}
		storage[file_name] = make_pair(make_pair(data,channels), make_pair(width, height));
	} else {
		data = it->second.first.first;
		channels = it->second.first.second;
		width = it->second.second.first;
		height = it->second.second.second;
	}
}

Vec Texture::get_color(real u, real v) {
	int w = int(u * (width-1));
	int h = int(v * (height-1));

	real r = real(data[(h * width + w) * channels])/255.0;
	real g = real(data[(h * width + w) * channels+1])/255.0;
	real b = real(data[(h * width + w) * channels+2])/255.0;

	return Vec(r,g,b);
}

#undef STB_IMAGE_IMPLEMENTATION
