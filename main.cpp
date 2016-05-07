#include "constants.h"
#include "vec.h"

#include <string>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
using namespace std;

enum Material{DIFF, SPEC, REFR};	
struct Sphere {
	double radius;
	Vec pos, light, color;
	Material material;
	Sphere(double _r, Vec _p, Vec _l, Vec _c, Material _m) :
		radius(_r), pos(_p), light(_l), color(_c), material(_m) {}
};

int sphere_total = 8;
Sphere spheres[] = {//Scene: radius, position, emission, color, material 
	Sphere(16.5,Vec(27,16.5,47),             Vec(),            Vec(1,1,1)*.999, SPEC),//ball1
	Sphere(16.5,Vec(57,46.5,47),             Vec(),            Vec(0.6,0.6,0.8),REFR),//ball2
	Sphere(1e5, Vec(0, 0, -(1e5)+47-16.5),   Vec(),            Vec(.25,.25,.25),DIFF),//bottom 
	Sphere(1e5, Vec(0, 0, (1e5)+107+16.5),   Vec(),            Vec(.25,.25,.25),DIFF),//top
//	Sphere(500, Vec(65, 26.5, (500)+106.7+16.5),Vec(1,1,1)*400,Vec(.25,.25,.25),DIFF),//light 
	Sphere(2,   Vec(65, 26.5, -4+106+16.5),Vec(1,1,1)*400,Vec(.25,.25,.25),DIFF),//light 
	Sphere(1e5, Vec(-(1e5)+17-16.5, 0, 0) ,  Vec(),            Vec(.75,.25,.25),DIFF),//left
	Sphere(1e5, Vec((1e5)+117+16.5, 0, 0) ,  Vec(),            Vec(.75,.75,.25),DIFF),//right 
	Sphere(1e5, Vec(0, -(1e5)-10-16.5, 0),   Vec(),            Vec(1,1,1)*.888, DIFF),//back

	Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left 
	Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght 
	Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back 
	Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt 
	Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm 
	Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top 
	Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr 
	Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas 
	Sphere(600, Vec(50,681.6-.27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite 
};

struct Ray {
	Vec pos, dir;
	Ray(Vec _p = Vec(), Vec _d = Vec()) :
		pos(_p), dir(_d) {}
	bool intersect_with_sphere(Sphere sphere, double& dist) {
		Vec _x = dir;
		Vec _y = sphere.pos - pos;
		double b = _x * _y;
		double _y_length = _y.length();
		double c = _y_length * _y_length - sphere.radius * sphere.radius;
		double delta = b * b - c;
		if (delta < 0) return false;
		delta = sqrt(delta);
		double eps = 1e-4;
		if (b - delta > eps) {
			dist = b - delta;
			return true;
		} else
		if (b + delta > eps) {
			dist = b + delta;
			return true;
		} else
			return false;
	}
};

double randf(double l, double r) {
	return (double)l + (double)rand() / RAND_MAX * (r - l);
}

double randi(int l,int r) {
	return l + rand() % (r - l + 1);
}

double sqr(double x) {
	return x * x;
}

// return -1 when no intersection
int intersect(Ray ray, double& min_dist) {
	double temp_dist;
	int nearest_sphere = -1;
	for (int i = 0; i < sphere_total; ++i)
		if (ray.intersect_with_sphere(spheres[i], temp_dist))
			if (nearest_sphere == -1 || temp_dist < min_dist) {
				min_dist = temp_dist;
				nearest_sphere = i;
			}
	return nearest_sphere;
}

Vec tracing(Ray ray, int depth, int explicit_parameter = 1) {
	int nearest_sphere;
	double min_dist;
	nearest_sphere = intersect(ray, min_dist);
	if (nearest_sphere == -1) return Vec();
	const Sphere& sphere = spheres[nearest_sphere];

	Vec color = sphere.color;
	double max_color = max(color.x,max(color.y,color.z));
	if (depth > 4 || !max_color) {
		if (randf(0, 1.0) < max_color) color = color * (1.0 / max_color);
		else return sphere.light * explicit_parameter;
	}

	Vec x = ray.pos + ray.dir * min_dist;
	Vec n = (x - sphere.pos).normal();
	Vec nl = (n * ray.dir < 0) ? n : n * (-1);

	if (sphere.material == DIFF) {
		double r1 = randf(0, 2.0 * M_PI);
		double r2 = randf(0, 1.0);
		double r2s = sqrt(r2);
		Vec w = nl;
		Vec u = (((abs(w.x) > 0.1) ? Vec(0, 1, 0) : Vec(1, 0, 0)) % w).normal();
		Vec v = w % u;
		Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normal();

		// explicit lighting
		Vec extra = Vec();
		for (int i = 0; i < sphere_total; ++i)
			if (spheres[i].light.x > 0 ||spheres[i].light.y > 0 ||spheres[i].light.z > 0) {
				Vec sw = (spheres[i].pos - x).normal();
				Vec su = (((abs(sw.x) > 0.1) ? Vec(0, 1, 0) : Vec(1, 0, 0)) % sw).normal();
				Vec sv = sw % su;
				double cos_a_max = sqrt(1 - sqr(spheres[i].radius) / ((spheres[i].pos - x) * (spheres[i].pos - x)));
				double cos_a = randf(cos_a_max, 1.0);
				double sin_a = sqrt(1.0 - sqr(cos_a));
				double phi = randf(0, 2.0 * M_PI);
				Vec l = (su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a).normal();
				double temp_min_dist;
				if (intersect(Ray(x, l), temp_min_dist) == i) {
					double omega = 2.0 * M_PI * (1.0 - cos_a_max);
					extra = extra + color.blend(spheres[i].light * (l * nl) * omega) * M_1_PI;
				}
			}

		return sphere.light * explicit_parameter + extra + color.blend(tracing(Ray(x, d), depth + 1, 0));
	} else
	if (sphere.material == SPEC) {
		Vec d = (n * (-ray.dir * n) * 2 + ray.dir).normal();
		return sphere.light + color.blend(tracing(Ray(x, d), depth + 1));
	} else {
		// material == REFR
		Ray reflect_ray = Ray(x, (n * (-ray.dir * n) * 2 + ray.dir).normal());
		bool go_into = true;
		double air_speed = 1.5;
		double solid_speed = 1.0;
		double refract_rate = air_speed / solid_speed;
		if (n * ray.dir > 0) {
			go_into = false;
			refract_rate = solid_speed / air_speed;
		}
		double cos_value = -ray.dir * nl;
		double sin_value = sqrt(1 - cos_value * cos_value);
		if (sin_value / refract_rate > 1)
			return sphere.light + color.blend(tracing(reflect_ray, depth + 1));
		else {
			sin_value /= refract_rate;
			cos_value = sqrt(1 - sin_value * sin_value);
			Vec u = -nl;
			Vec v = (nl * (-ray.dir * nl) + ray.dir).normal();
			Ray refract_ray = Ray(x, (u * cos_value + v * sin_value).normal());
			double a = air_speed - solid_speed;
			double b = air_speed + solid_speed;
			double c = 1 - (go_into ? -(ray.dir * nl) : (refract_ray.dir * n));
			double R0 = a * a / b / b;
 			double Re = R0 + (1 - R0) * c * c * c * c * c;
 			double Tr = 1 - Re;
 			double P = 0.25 + 0.5 * Re;
 			double RP = Re / P;
 			double TP = Tr / (1 - P); 
 			if (depth <= 2)
 				return sphere.light + color.blend(tracing(reflect_ray, depth + 1) * Re + tracing(refract_ray, depth + 1) * Tr);
 			else if (randf(0, 1.0) < P)
 				return sphere.light + color.blend(tracing(reflect_ray, depth + 1) * RP);
 			else
 				return sphere.light + color.blend(tracing(refract_ray, depth + 1) * TP);
		}
	}
}

Vec pic[HEIGHT][WIDTH];

double truncate(double value) {
	if (value > 1) return 1.0;
	if (value < 0) return 0;
	return value;
}

int pixel_int(double value) {
	return int(pow(truncate(value), 1.0 / 2.2) * 255.0 + 0.5);
}

void output_picture(int sample_times) {
	string file_name = string("output/image") + to_string(sample_times) + string(".ppm");
	FILE* file = fopen(file_name.c_str(), "w");
	fprintf(file, "P3\n%d %d\n%d\n", WIDTH, HEIGHT, 255);
	for (int h = 0; h < HEIGHT; ++h)
		for (int w = 0; w < WIDTH; ++w) {
			int r = pixel_int(pic[h][w].x / sample_times);
			int g = pixel_int(pic[h][w].y / sample_times);
			int b = pixel_int(pic[h][w].z / sample_times);
			fprintf(file, "%d %d %d ", r, g, b);
		}
}

int main() {
	Ray camera = Ray(Vec(67, 180, 77), Vec(0, -1, 0));
	int sample_times = 1;
	int interval = 10;
	for (int sample_times = 1; ; ++sample_times) {
		time_t start_time=time(0);
		for (int h = 0; h < HEIGHT; ++h) {
			for (int w = 0; w < WIDTH; ++w) {
				double x = (randf(-1.0, 1.0) + w - WIDTH / 2) / WIDTH;
				double z = (randf(-1.0, 1.0) - h + HEIGHT / 2) / WIDTH;
				Ray ray = Ray(camera.pos, Vec(x ,-1, z).normal());
				pic[h][w] = pic[h][w] + tracing(ray, 0);
			}
		}
		time_t end_time=time(0);
		printf("The %d time to sample. Time cost is %ld s\n", sample_times, end_time - start_time);

		if (sample_times % interval == 0) {
			output_picture(sample_times);
			if (sample_times / interval == 10)
				interval *= 10;
		}
	}
	return 0;
}