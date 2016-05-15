#include "constants.h"
#include "vec.h"
#include "ray.h"
#include "model.h"
#include "sphere.h"
#include "hpoint.h"
#include "grid.h"
#include "texture.h"

#include <string>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <utility>
using namespace std;

// color of light source must be Vec()
int sphere_total = 0;
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

// xorshift PRNG
float randf() {
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	unsigned int t = x ^ (x << 11); x = y; y = z; z = w;
	return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) ) * (1.0f / 4294967296.0f);
}

float sqr(float x) {
	return x * x;
}

// return -1 when no intersection
// reture -2 when intersecting with model
Texture texture("pictures/glass.png");
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

			if (x.x > 50.0 - 1e-4 && x.y > 1e-4) {
				float u = x.y/50.0;
				float v = (x.z+50)/100.0;
				if (0.05<u && u<0.95 && 0.05<v && v<0.95) {
					c = texture.get_color(u,v);
				} else
					c = Vec(0.15,0.15,0.15);
			}

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
	const float offset_eps = 1e-3;

	vector< Vec > colors(1, v);
	vector< Ray > rays(1, ray);
	vector< int > depths(1, 0);

	for (int l=0; l < colors.size(); ++l) {
		if (depths[l] > 20)
			continue;

		ray = rays[l];
		if (eye_ray && depths[l]==0)
			ray.pos = ray.pos + ray.dir * 100.0;

		// information about the intersection
		// without light
		Vec x, n, color;
		Material material;
		
		int nearest_sphere = intersect(ray, x, n, color, material);

		if (nearest_sphere == -1)
			continue;

		Vec nl = (n * ray.dir < 0) ? n : n * (-1);
		float max_color = max(color.x,max(color.y,color.z));


		// volumn light
		
		float sigma = 0.05;
		float dist = (x-ray.pos).length();
		if (eye_ray) {
			float stop_probability = 1.0 - exp(-dist*sigma);
			float interval = 1.0;
			float importance_sum = 0;
			vector< pair<float,float> > samples;
			for (float i = interval; i < dist; i += interval) {
				float stop_dist = i - randf()*interval;
				float importance = sigma * exp(-stop_dist*sigma);
				importance_sum += importance;
				samples.push_back(make_pair(stop_dist,importance));
			}
			for (int i = 0; i < samples.size(); ++i) {
				float stop_dist = samples[i].first;
				float importance = samples[i].second * stop_probability / importance_sum;
				points.push_back(HPoint(ray.pos+ray.dir*stop_dist, n, color.blend(colors[l])*importance,pixel_h,pixel_w));
				//points.push_back(HPoint(ray.pos+(x-ray.pos)*randf(), n, color.blend(colors[l])*stop_probability*(interval/dist),pixel_h,pixel_w));
			}
			colors[l] = colors[l] * (1.0 - stop_probability);
		} else {
			float stop_dist = -log(1.0-randf()) / sigma;
//			if (depths[l] == 0 && stop_dist < dist) {
			if (stop_dist < dist) {
				x = ray.pos + ray.dir * stop_dist;
				vector<HPoint*>& points = grid->find_possible_near_points(x);
				for (int i = 0; i < (points).size(); ++i) {
					HPoint* point = points[i];
					Vec v = point->x - x;
					if (v*v <= point->r2) {
						float g = (1.0*point->cnt*ALPHA+ALPHA) / (1.0*point->cnt*ALPHA+1.0);
						point->r2 *= g;
						point->cnt ++;
						point->flux = (point->flux + point->c.blend(colors[l])*(1.0/M_PI)) * g;
					}
				}
				continue;
			}
		}
		
		


		if (material == DIFF) {
			if (eye_ray) {
				points.push_back(HPoint(x, n, color.blend(colors[l]), pixel_h, pixel_w));
//				if (200 < pixel_h && pixel_h < 300 && 200 < pixel_w && pixel_w < 300)
//					fprintf(stderr, "%.2f %.2f %.2f      %.2f %.2f %.2f\n",x.x,x.y,x.z,n.x,n.y,n.z);
			} else {
				vector<HPoint*>& points = grid->find_possible_near_points(x);
				for (int i = 0; i < (points).size(); ++i) {
					HPoint* point = points[i];
					Vec v = point->x - x;
					if ((point->n*n>1e-3f) && (v*v <= point->r2)) {
						float g = (1.0*point->cnt*ALPHA+ALPHA) / (1.0*point->cnt*ALPHA+1.0);
						point->r2 *= g;
						point->cnt ++;
						point->flux = (point->flux + point->c.blend(colors[l])*(1.0/M_PI)) * g;
					}
				}
				if (randf() < max_color) {
					float r1 = randf() * 2.0f * M_PI;
					float r2 = randf();
					float r2s = sqrt(r2);
					Vec w = nl;
					Vec u = (((fabs(w.x) > 0.1) ? Vec(0, 1, 0) : Vec(1, 0, 0)) % w).normal();
					Vec v = w % u;
					Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normal();
					colors.push_back(color.blend(colors[l])*(1.0/max_color));
					rays.push_back(Ray(x + d * offset_eps, d));
					depths.push_back(depths[l]+1);
				}
			}
		} else
		if (material == SPEC) {
			Vec d = (n * (-ray.dir * n) * 2 + ray.dir).normal();
//			return light + color.blend(tracing(Ray(x, d), depth + 1));
			colors.push_back(color.blend(colors[l]));
			rays.push_back(Ray(x + d * offset_eps, d));
			depths.push_back(depths[l]+1);
		} else {
			// material == REFR
			Vec reflect_dir = (n * (-ray.dir * n) * 2 + ray.dir).normal();
			Ray reflect_ray = Ray(x + reflect_dir * offset_eps, reflect_dir);
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
				Vec refract_dir = (u * cos_value + v * sin_value).normal();
				Ray refract_ray = Ray(x + refract_dir * offset_eps, refract_dir);
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
	 				if (randf() < P) {
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
	light = Vec(1.0,1.0,1.0) * 2500 * M_PI * 4.0;
	float p = randf() * 2.0 * M_PI;
	float t = 2.0 * acos(sqrt(randf()));
	double st = sin(t);
//	photon = Ray(Vec(0, -25, 42.5), Vec(cos(p)*st, sin(p)*st, cos(t)));

//	photon = Ray(Vec(49.0+randf(), 49.0+randf(), 50), Vec(0, sin(p)*st, cos(t)));
//	photon = Ray(Vec(48.0+randf()*2, 48.0+randf()*2, 49.5), Vec(-3, -2, -4).normal());
//	photon = Ray(Vec(49.5, 49.5, 49.5), Vec(-2+sin(p)*0.12*st, -1+cos(p)*0.12*st, -4).normal());

	float a = randf()*M_PI/3.0;
	float u = randf();
	float v = 1.0-tan(a)/sqrt(3.0);
	if (0.05<u && u<0.95 && 0.05<v && v<0.95) {
		photon = Ray(Vec(49.9, u*50, v*100-50), Vec(-cos(a)+randf()*0.05-0.025,randf()*0.05-0.025,-sin(a)+randf()*0.05).normal());
		light = texture.get_color(u,v) * 500000 * M_PI * 4.0 * v * v;
	} else {
		photon = Ray(Vec(randf()*100-50, randf()*50, randf()*100-50), Vec(cos(p)*st, sin(p)*st, cos(t)));
		light = Vec(0.10,0.10,0.10) * 500000 * M_PI * 4.0;
	}
}

int main() {
	model.load_from_obj("models/bunny.fine.obj");

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

	int interval = 10000;
	time_t last_check_time=time(0);
	for (int photon_number = 1; ; ++photon_number) {
		fprintf(stderr,"\rRecent photon number is %d",photon_number);
		Ray ray;
		Vec color;
		generate_photon(ray, color);
		trace(ray, color, false);
		// output to screen
		if (photon_number % 100000 == 0) {
			printf("\rPhoton number is %dw, the time cost is %lds\n", photon_number/10000, time(0)-last_check_time);
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