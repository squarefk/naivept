#include "constants.h"
#include "vec.h"
#include "ray.h"
#include "model.h"
#include "sphere.h"
#include "hpoint.h"
#include "grid.h"

#include <string>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <utility>
using namespace std;

// color of light source must be Vec()
int sphere_total = 1;
Sphere spheres[] = {//Scene: radius, position, emission, color, material 
	Sphere(15,Vec(-30, 20 ,-35),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr 

	Sphere(5,   Vec(0, 25, 42.5),  Vec(1,1,1)*50,   Vec(),           DIFF),//light 

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

Model model;

float randf(float l, float r) {
	return (float)l + (float)rand() / RAND_MAX * (r - l);
}

float randi(int l,int r) {
	return l + rand() % (r - l + 1);
}

float sqr(float x) {
	return x * x;
}

// return -1 when no intersection
// reture -2 when intersecting with model
int intersect(Ray ray, Vec& x, Vec& n, Vec& c, Material& m) {
	float min_dist, temp_dist;
	int nearest_sphere = -1;
	
	for (int i = 0; i < sphere_total; ++i)
		if (ray.intersect_with_sphere(spheres[i], temp_dist))
			if (nearest_sphere == -1 || temp_dist < min_dist) {
				min_dist = temp_dist;
				nearest_sphere = i;
				x = ray.pos + ray.dir * min_dist;
				n = (x-spheres[i].pos).normal();
				c = spheres[i].color;
				m = spheres[i].material;
			}
	
	float model_dist;
	Vec model_n, model_c;
	Material model_m;
	if (model.intersect(ray, model_dist, model_n, model_c, model_m))
		if (nearest_sphere == -1 || model_dist < min_dist) {
			x = ray.pos + ray.dir * model_dist;
			n = model_n;
			c = model_c;
			m = model_m;
			return -2;
		}
	return nearest_sphere;
}

vector <HPoint> points;
Grid* grid;
int pixel_h, pixel_w;
// ATTENTION: no light now
// eye_ray == false represents photon_ray
void trace(Ray ray, Vec v, bool eye_ray) {
	vector< Vec > colors(1, v);
	vector< Ray > rays(1, ray);
	vector< int > depths(1, 0);

	for (int l=0; l < colors.size(); ++l) {
		if (depths[l] > 20)
			continue;

		ray = rays[l];

		// information about the intersection
		// without light
		Vec x, n, color;
		Material material;
		
		int nearest_sphere = intersect(ray, x, n, color, material);
		if (nearest_sphere == -1)
			continue;

		Vec nl = (n * ray.dir < 0) ? n : n * (-1);
		float max_color = max(color.x,max(color.y,color.z));

		if (material == DIFF) {
			if (eye_ray) {
				points.push_back(HPoint(x, n, color.blend(colors[l]), pixel_h, pixel_w));
			} else {
				vector<HPoint>& points = grid->find_possible_near_points(x);
				for (int i = 0; i < (points).size(); ++i) {
					HPoint& point = points[i];
					Vec v = point.x - x;
//					fprintf(stderr, "ERRROR%.2f %.2f\n",v*v,point.r2);
					if ((point.n*n>1e-3f) && (v*v <= point.r2)) {
						float g = (point.cnt*ALPHA+ALPHA) / (point.cnt*ALPHA+1.0);
						point.r2 *= g;
						point.cnt ++;
						point.flux = (point.flux + point.c.blend(colors[l])*(1.0/M_PI)) * g;
//						fprintf(stderr,"ERRROROOR %.5f %.5f %.5f\n",point.flux.x,point.flux.y,point.flux.z);
//						fprintf(stderr,"ERRROROOR %d %d\n",point.h,point.w);
					}
				}
				if (randf(0.0, 1.0) < max_color) {
					float r1 = randf(0, 2.0 * M_PI);
					float r2 = randf(0, 1.0);
					float r2s = sqrt(r2);
					Vec w = nl;
					Vec u = (((abs(w.x) > 0.1) ? Vec(0, 1, 0) : Vec(1, 0, 0)) % w).normal();
					Vec v = w % u;
					Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normal();
					colors.push_back(color.blend(colors[l])*(1.0/max_color));
					rays.push_back(Ray(x, d));
					depths.push_back(depths[l]+1);
				}
			}
		} else
		if (material == SPEC) {
			Vec d = (n * (-ray.dir * n) * 2 + ray.dir).normal();
//			return light + color.blend(tracing(Ray(x, d), depth + 1));
			colors.push_back(color.blend(colors[l]));
			rays.push_back(Ray(x, d));
			depths.push_back(depths[l]+1);
		} else {
			// material == REFR
			Ray reflect_ray = Ray(x, (n * (-ray.dir * n) * 2 + ray.dir).normal());
			bool go_into = true;
			float air_speed = 1.0, solid_speed = 0.75;
			float refract_rate = air_speed / solid_speed;
			if (n * ray.dir > 0) {
				go_into = false;
				refract_rate = solid_speed / air_speed;
			}
			float cos_value = -ray.dir * nl;
			float sin_value = sqrt(1 - cos_value * cos_value);
			if (sin_value / refract_rate > 1) {
				// return light + color.blend(tracing(reflect_ray, depth + 1));
				// ATTENTION: not blend color?
				colors.push_back(color.blend(colors[l]));
				rays.push_back(reflect_ray);
				depths.push_back(depths[l]+1);	
			} else {
				sin_value /= refract_rate;
				cos_value = sqrt(1 - sin_value * sin_value);
				Vec u = -nl;
				Vec v = (nl * (-ray.dir * nl) + ray.dir).normal();
				Ray refract_ray = Ray(x, (u * cos_value + v * sin_value).normal());
				float a = air_speed - solid_speed;
				float b = air_speed + solid_speed;
				float c = 1 - (go_into ? -(ray.dir * nl) : (refract_ray.dir * n));
				float R0 = a * a / b / b;
	 			float Re = R0 + (1 - R0) * c * c * c * c * c;
	 			float P = Re;
	 			if (eye_ray) {
					colors.push_back(color.blend(colors[l])*Re);
					rays.push_back(reflect_ray);
					depths.push_back(depths[l]+1);
					// === cut line ========
					colors.push_back(color.blend(colors[l])*(1.0-Re));
					rays.push_back(refract_ray);
					depths.push_back(depths[l]+1);
	 			} else {
	 				if (randf(0, 1.0) < P) {
						colors.push_back(color.blend(colors[l]));
						rays.push_back(reflect_ray);
						depths.push_back(depths[l]+1);
		 			} else {
						colors.push_back(color.blend(colors[l]));
						rays.push_back(refract_ray);
						depths.push_back(depths[l]+1);
	 				}
	 			}
			}
		}
	}
}

void generate_photon(Ray& photon, Vec& light) {
	light = Vec(2500,2500,2500) * M_PI * 4.0;
	float p = randf(0.0, 2.0 * M_PI);
	float t = 2.0 * acos(sqrt(randf(0.0, 1.0)));
	double st = sin(t);
	photon = Ray(Vec(0, 25, 42.5), Vec(cos(p)*st, cos(t), sin(p)*st));
}

int main() {
	model.load_from_obj("models/water/water.obj");

	Ray camera = Ray(Vec(0, -100, 0), Vec(0, 1, 0));

	// eye ray trac
	for (int h = 0; h < HEIGHT; ++h) {
		fprintf(stderr, "\rEye ray tracing %.2f%%", 100.0f * (h+1) / HEIGHT);
		for (int w = 0; w < WIDTH; ++w) {
			pixel_h = h;
			pixel_w = w;
			float x = (1.0 * w - WIDTH / 2) / WIDTH;
			float z = (-1.0 * h + HEIGHT / 2) / WIDTH;
			Ray ray = Ray(camera.pos, Vec(x ,1, z).normal());
			trace(ray, Vec(1.0, 1.0, 1.0), true);
		}
	}
	puts("");
	grid = new Grid(points, HEIGHT, WIDTH);

	int interval = 100000;
	time_t last_check_time=time(0);
	for (int photon_number = 1; ; ++photon_number) {
		Ray ray;
		Vec color;
		generate_photon(ray, color);
		trace(ray, color, false);
		// output to screen
		if (photon_number % 100000 == 0) {
			printf("Photon number is %dw. The time cost is %lds\n", photon_number/10000, time(0)-last_check_time);
			last_check_time = time(0);
		}
		// output picture
		if (photon_number % interval == 0) {
			grid->output_picture(photon_number);
			if (photon_number / interval == 10)
				interval *= 10;
		}
	}

	return 0;
}