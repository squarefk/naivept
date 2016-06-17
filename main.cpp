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
	Sphere(20,Vec(27.5, 25 ,-30),       Vec(),Vec(1,1,1)*.99, REFR),//Mirr 
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
real randf() {
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	unsigned int t = x ^ (x << 11); x = y; y = z; z = w;
	return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) ) * (1.0f / 4294967296.0f);
}

real sqr(real x) {
	return x * x;
}

// return -1 when no intersection
// reture -2 when intersecting with model
Texture texture("pictures/glass.png");
int intersect(Ray ray, Vec& x, Vec& n, Vec& c, Material& m) {
	real min_dist, temp_dist;
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
	
	real model_dist;
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
Grid grid;
int pixel_h, pixel_w;
real global_r2;
// ATTENTION: no light now
// eye_ray == false represents photon_ray
void trace(Ray ray, Vec v, bool eye_ray) {
	const real offset_eps = 1e-4;

	vector< Vec > colors(1, v);
	vector< Ray > rays(1, ray);
	vector< int > depths(1, 0);
//	fprintf(stderr, "\n\n==========\n\n");
	for (int l=0; l < colors.size(); ++l) {
		if (depths[l] > 20)
			continue;

		ray = rays[l];
//		if (eye_ray && depths[l]==0)
//			ray.pos = ray.pos + ray.dir * 100.0;

		// information about the intersection
		// without light
		Vec x, n, color;
		Material material;

		int nearest_sphere = intersect(ray, x, n, color, material);

		if (nearest_sphere == -1) {
			continue;
		}

		Vec nl = (n * ray.dir < 0) ? n : n * (-1);
		real max_color = max(color.x,max(color.y,color.z));

		// volumn light
		/*
		real sigma = 0.05;
		real dist = (x-ray.pos).length();
		if (eye_ray) {
			real stop_probability = 1.0 - exp(-dist*sigma);
			real interval = 1.0;
			real importance_sum = 0;
			vector< pair<real,real> > samples;
			for (real i = interval; i < dist; i += interval) {
				real stop_dist = i - randf()*interval;
				real importance = sigma * exp(-stop_dist*sigma);
				importance_sum += importance;
				samples.push_back(make_pair(stop_dist,importance));
			}
			for (int i = 0; i < samples.size(); ++i) {
				real stop_dist = samples[i].first;
				real importance = samples[i].second * stop_probability / importance_sum;
				points.push_back(HPoint(ray.pos+ray.dir*stop_dist, n, color.blend(colors[l])*importance,pixel_h,pixel_w));
				//points.push_back(HPoint(ray.pos+(x-ray.pos)*randf(), n, color.blend(colors[l])*stop_probability*(interval/dist),pixel_h,pixel_w));
			}
			colors[l] = colors[l] * (1.0 - stop_probability);
		} else {
			real stop_dist = -log(1.0-randf()) / sigma;
//			if (depths[l] == 0 && stop_dist < dist) {
			if (stop_dist < dist) {
				x = ray.pos + ray.dir * stop_dist;
				vector<HPoint*>& points = grid.find_possible_near_points(x);
				for (int i = 0; i < (points).size(); ++i) {
					HPoint* point = points[i];
					Vec v = point->x - x;
					if (v*v <= global_r2) {
						real g = (1.0*point->cnt*ALPHA+ALPHA) / (1.0*point->cnt*ALPHA+1.0);
						point->cnt ++;
						point->flux = (point->flux + point->c.blend(colors[l])*(1.0/M_PI)) * g;
					}
				}
				// multiple scattering
				real absorption = 0.5;
				if (randf() < 1.0 - absorption) {
					real p = randf() * 2.0 * M_PI;
					real t = 2.0 * acos(sqrt(randf()));
					real st = sin(t);
					colors.push_back(colors[l]);
					rays.push_back(Ray(x, Vec(cos(p)*st, sin(p)*st, cos(t))));
					depths.push_back(depths[l]+1);
				}
				continue;
			}
		}
		*/
		
//		fprintf(stderr, "%d  %.2f %.2f %.2f      %.2f %.2f %.2f      %.2f %.2f %.2f\n",depths[l], x.x,x.y,x.z,ray.pos.x,ray.pos.y,ray.pos.z,ray.dir.x,ray.dir.y,ray.dir.z);
//		if (eye_ray && pixel_h==414 && pixel_w==288)
//			fprintf(stderr,"\n%.5f %.5f %.5f %d       %.5f %.5f %.5f  (%d)\n",x.x,x.y,x.z,material,colors[l].x,colors[l].y,colors[l].z,pixel_w);
//		if (eye_ray && pixel_h==414 && pixel_w==289)
//			fprintf(stderr,"\n%.5f %.5f %.5f %d       %.5f %.5f %.5f  (%d)\n",x.x,x.y,x.z,material,colors[l].x,colors[l].y,colors[l].z,pixel_w);
//		if (eye_ray && pixel_h==414 && pixel_w==288 && depths[l]>1)
//			continue;
		if (material == CERA && eye_ray) {
			real prop = 0.5;
			points.push_back(HPoint(x, n, color.blend(colors[l]) * prop, pixel_h, pixel_w));

			Vec d = (n * (-ray.dir * n) * 2 + ray.dir).normal();
//			return light + color.blend(tracing(Ray(x, d), depth + 1));
			colors.push_back(color.blend(colors[l]) * (1 - prop));
			rays.push_back(Ray(x + nl * offset_eps, d));
			depths.push_back(depths[l]+1);
		} else
		if (material == DIFF || material == CERA) {
			if (eye_ray) {
				points.push_back(HPoint(x, n, color.blend(colors[l]), pixel_h, pixel_w));
			} else {
				vector<HPoint*>& points = grid.find_possible_near_points(x);
				for (int i = 0; i < (points).size(); ++i) {
					HPoint* point = points[i];
					Vec v = point->x - x;
					if ((point->n*n>1e-3f) && (v*v <= global_r2)) {
						real g = (1.0*point->cnt*ALPHA+ALPHA) / (1.0*point->cnt*ALPHA+1.0);
						point->cnt ++;
						point->flux = (point->flux + point->c.blend(colors[l])*(1.0/M_PI)) * g;
					}
				}
				if (randf() < max_color) {
					real r1 = randf() * 2.0f * M_PI;
					real r2 = randf();
					real r2s = sqrt(r2);
					Vec w = nl;
					Vec u = (((fabs(w.x) > 0.1) ? Vec(0, 1, 0) : Vec(1, 0, 0)) % w).normal();
					Vec v = w % u;
					Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).normal();
					colors.push_back(color.blend(colors[l])*(1.0/max_color));
					rays.push_back(Ray(x + nl * offset_eps, d));
					depths.push_back(depths[l]+1);
				}
			}
		} else
		if (material == SPEC) {
			Vec d = (n * (-ray.dir * n) * 2 + ray.dir).normal();
//			return light + color.blend(tracing(Ray(x, d), depth + 1));
			colors.push_back(color.blend(colors[l]));
			rays.push_back(Ray(x + nl * offset_eps, d));
			depths.push_back(depths[l]+1);
		} else
		if (material == REFR) {
			Vec reflect_dir = (ray.dir - n * 2 * (ray.dir * n)).normal();
			Ray reflect_ray = Ray(x + nl * offset_eps, reflect_dir);

			bool into = (n * nl > 0);
			real nc=1.0, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=ray.dir*nl;
			real cos2t=1-nnt*nnt*(1-ddn*ddn);
			if (cos2t < 0) {
				// return light + color.blend(tracing(reflect_ray, depth + 1));
				// ATTENTION: not blend color?
				colors.push_back(colors[l]);
				rays.push_back(reflect_ray);
				depths.push_back(depths[l]+1);
			} else {
				Vec refract_dir = (ray.dir*nnt-n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).normal();
				Ray refract_ray = Ray(x - nl * offset_eps, refract_dir);

				real a=nt-nc, b=nt+nc, c=1-(into?-ddn:refract_dir*n);
				real R0=a*a/(b*b), Re=R0+(1-R0)*c*c*c*c*c, P = Re;
	 			if (eye_ray) {
					colors.push_back(color.blend(colors[l])*Re);
					rays.push_back(reflect_ray);
					depths.push_back(depths[l]+1);
					colors.push_back(color.blend(colors[l])*(1.0-Re));
					rays.push_back(refract_ray);
					depths.push_back(depths[l]+1);
	 			} else {
	 				if (randf() < P) {
						colors.push_back(colors[l]);
						rays.push_back(reflect_ray);
						depths.push_back(depths[l]+1);
		 			} else {
						colors.push_back(colors[l]);
						rays.push_back(refract_ray);
						depths.push_back(depths[l]+1);
	 				}
	 			}
			}
		}
	}
//	if (colors.size()>1)
//		fprintf(stderr, "========\n");
}

void generate_photon(Ray& photon, Vec& light) {
	light = Vec(1.0,1.0,1.0) * 2500 * M_PI * 4.0;
	real p = randf() * 2.0 * M_PI;
	real t = 2.0 * acos(sqrt(randf()));
	real st = sin(t);
//	photon = Ray(Vec(0, -25, 42.5), Vec(cos(p)*st, sin(p)*st, cos(t)));

//	photon = Ray(Vec(49.0+randf(), 49.0+randf(), 50), Vec(0, sin(p)*st, cos(t)));
//	photon = Ray(Vec(48.0+randf()*2, 48.0+randf()*2, 49.5), Vec(-3, -2, -4).normal());
//	photon = Ray(Vec(49.5, 49.5, 49.5), Vec(-2+sin(p)*0.12*st, -1+cos(p)*0.12*st, -4).normal());

	real a = randf()*M_PI/3.0;
	real u = randf();
	real v = 1.0-tan(a)/sqrt(3.0);
//	photon = Ray(Vec(49.9, u*50, v*100-50), Vec(-cos(a)+randf()*0.05-0.025,randf()*0.05-0.025,-sin(a)+randf()*0.05).normal());
//	light = texture.get_color(u,v) * 500000 * M_PI * 4.0 * v * v;

//	photon = Ray(Vec(0, randf()*10+20, randf()*10+45), Vec(cos(p)*st, sin(p)*st, cos(t)));
	photon = Ray(Vec(randf()*20-10, randf()*20+30, 49.9), Vec(cos(p)*st, sin(p)*st, cos(t)));
//	photon = Ray(Vec(randf()*100-50, randf()*50, randf()*100-50), Vec(cos(p)*st, sin(p)*st, cos(t)));
	light = Vec(1.0,1.0,1.0) * 2500 * M_PI * 4.0;
}

int main() {
	model.load_from_obj("models/bunny.fine.obj");

	Ray camera = Ray(Vec(0, -100, 0), Vec(0, 1, 0));

	for (int epochs_number = 1; epochs_number < 100; ++epochs_number) {
		fprintf(stderr, "================== Epochs %d ==================\n", epochs_number);

		points.clear();

		// eye ray trace
		for (int h = 0; h < HEIGHT; ++h) {
			fprintf(stderr, "\rEye ray tracing %.2f%%", 100.0f * (h+1) / HEIGHT);
			for (int w = 0; w < WIDTH; ++w) {
				pixel_h = h;
				pixel_w = w;
				real x = (1.0 * w + randf() - 0.5 - WIDTH / 2) / WIDTH;
				real z = (-1.0 * h + randf() - 0.5  + HEIGHT / 2) / WIDTH;
				Ray ray = Ray(camera.pos, Vec(x ,1, z).normal());
				trace(ray, Vec(1.0, 1.0, 1.0), true);
			}
		}

		grid.build(points, HEIGHT, WIDTH, epochs_number, global_r2);

		int interval = 100000;
		int total_photon_number = 1e7;
		time_t last_check_time=time(0);
		for (int photon_number = 1; photon_number <= total_photon_number; ++photon_number) {
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
				grid.output_picture(photon_number, epochs_number, global_r2);
				if (photon_number / interval == 10)
					interval *= 10;
			}
		}

		grid.save_picture(total_photon_number, global_r2);
	}

	return 0;
}