#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "tools/stb_image.h"

#include <algorithm>

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
		for (int i = 0; i < height; ++i)
			for (int j = 0; j < width; ++j) {
				real phi=2.0*M_PI*j/width, theta=(M_PI*(0.5+i))/height;
				Vec color = Vec(
					real(data[(i * width + j) * channels])/255.0,
					real(data[(i * width + j) * channels+1])/255.0,
					real(data[(i * width + j) * channels+2])/255.0
				);
				photon_p.push_back(sin(theta)*(0.299*color.x+0.587*color.y+0.114*color.z));
				photon_pos.push_back(Vec(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta))*1000);
				photon_dir.push_back(-Vec(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta)));
				photon_color.push_back(color);
			}
		for (int i = 1; i < height * width; ++i)
			photon_p[i] += photon_p[i-1];
	} else {
		data = it->second.first.first;
		channels = it->second.first.second;
		width = it->second.second.first;
		height = it->second.second.second;
	}
}

Vec Texture::get_color(real u, real v) {
	u = u - floor(u);
	v = v - floor(v);
	if (u<0 || u>1 || v<0 || v>1) {
		printf("Error uv address : %.2f %.2f\n",u,v);
		return Vec(0,0,0);
	}
	int w = int(u * (width-1));
	int h = int(v * (height-1));

	real r = real(data[(h * width + w) * channels])/255.0;
	real g = real(data[(h * width + w) * channels+1])/255.0;
	real b = real(data[(h * width + w) * channels+2])/255.0;

	return Vec(r,g,b);
}

Vec Texture::get_color(Vec dir) {
	real u = atan2(dir.y, dir.x) / M_PI / 2;
	real v = atan2(sqrt(dir.x * dir.x + dir.y * dir.y),dir.z) / M_PI;
	if (u < 0) u += 1.0;
	return get_color(u, v);
}

void Texture::generate_photon(Ray& photon, Vec& color, real rand_num) {
	rand_num *= photon_p[photon_p.size()-1];
	int id = lower_bound(photon_p.begin(), photon_p.end(), rand_num) - photon_p.begin();
	photon = Ray(photon_pos[id], photon_dir[id]);
	color = photon_color[id];
}

#undef STB_IMAGE_IMPLEMENTATION
