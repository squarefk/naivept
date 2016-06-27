#include "model.h"

#include "vec.h"
#include "ray.h"
#include "triangle.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
using namespace std;

bool cmpx(Triangle a, Triangle b) {return max(a.p.x,max(a.q.x,a.r.x)) < max(b.p.x,max(b.q.x,b.r.x));}
bool cmpy(Triangle a, Triangle b) {return max(a.p.y,max(a.q.y,a.r.y)) < max(b.p.y,max(b.q.y,b.r.y));}
bool cmpz(Triangle a, Triangle b) {return max(a.p.z,max(a.q.z,a.r.z)) < max(b.p.z,max(b.q.z,b.r.z));}

void Model::build(int k, int l, int r) {
	t[k].l=l;
	t[k].r=r;
	t[k].m=(l+r)>>1;
	t[k].low=t[k].high=triangles[l].p;
	for (int i=l;i<=r;++i) {
		const Triangle& tri = triangles[i];
		t[k].low.x=min(min(t[k].low.x,tri.p.x),min(tri.q.x,tri.r.x));
		t[k].low.y=min(min(t[k].low.y,tri.p.y),min(tri.q.y,tri.r.y));
		t[k].low.z=min(min(t[k].low.z,tri.p.z),min(tri.q.z,tri.r.z));
		t[k].high.x=max(max(t[k].high.x,tri.p.x),max(tri.q.x,tri.r.x));
		t[k].high.y=max(max(t[k].high.y,tri.p.y),max(tri.q.y,tri.r.y));
		t[k].high.z=max(max(t[k].high.z,tri.p.z),max(tri.q.z,tri.r.z));
	}
//	printf("(%.2f %.2f %.2f)   (%.2f %.2f %.2f) %d\n",t[k].low.x,t[k].low.y,t[k].low.z,t[k].high.x,t[k].high.y,t[k].high.z,k);
	real lenx = t[k].high.x - t[k].low.x;
	real leny = t[k].high.y - t[k].low.y;
	real lenz = t[k].high.z - t[k].low.z;
	sort(triangles.begin()+l,triangles.begin()+r+1, (lenx>leny && lenx>lenz)?cmpx:(leny>lenz?cmpy:cmpz));
	if (l == r) return;
	build(k+k,l,t[k].m);
	build(k+k+1,t[k].m+1,r);
}

bool Model::intersect(const Ray& ray, real& d, Vec& n, Vec& c, Material& m) {
	bool found = false;
	queue[1]=1;
	for (int l = 1, r = 1; l <= r; ++l) {
		int k = queue[l];
		if (t[k].l==t[k].r) {
			real dist;
			Vec norm;
			bool ret = ray.intersect_with_triangle(triangles[t[k].l], dist, norm);
			if (ret && dist>0 && (!found || dist < d)) {
				found = true;
				d = dist;
				n = norm;
				c = triangles[t[k].l].c;
				m = triangles[t[k].l].m;
			}
			continue;
		}
		if (!ray.intersect_with_box(t[k].low, t[k].high)) continue;
		queue[++r] = k+k;
		queue[++r] = k+k+1;
	}
	return found;
}


real randfa() {
	static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
	unsigned int t = x ^ (x << 11); x = y; y = z; z = w;
	return ( w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)) ) * (1.0f / 4294967296.0f);
}

void Model::load_from_obj(const char* file_name) {
	FILE* fp =fopen(file_name, "r");
	if (fp == NULL) {
		fprintf(stderr, "No object file found.\n");
		exit(-1);
	}
	fprintf(stderr, "Start loading obj from %s\n", file_name);

	char buf[25600];
	int lineNumber = 0;
	vector<Vec> v, vv, vvv;
	vector<Vec> n;
	vector<int> idx, idy, idz;
	real base = 50.0;
	while (fscanf(fp, "%s", buf) != EOF) {
		if (buf[0] == '#') {
			fgets(buf, sizeof(buf), fp);
		} else if (buf[0] == 'v') {
			real x, y, z, xx, yy, zz, xxx, yyy, zzz;
			fscanf(fp, "%lf %lf %lf", &x, &y, &z);
			real tx=x, ty=y, tz=z;
			// sphere
			x = tx * 20 -27.5;
			y = -tz * 20 + 25;
			z = ty * 20 - 30;
			// block
			x = tx * 20 - 10;
			y = -tz * 20 - 10;
			z = ty * 20 - 10;
			// water
			x = tx - 50;
			y = ty;
			z = tz - 15;
			// angel
			real theta=M_PI*3/4;
			x = -(tx*sin(theta)+tz*cos(theta))*0.25;
			y = (tx*cos(theta)-tz*sin(theta))*0.25+25;
			z = ty*0.25-50;

			// dragon
			double scale = 30;
			x = tx * scale - 30;
			y = -tz * scale - 30;
			z = ty * scale + 21.14868;

			xx = tx * scale + 10;
			yy = -tz * scale + 10;
			zz = ty * scale + 21.14868;

			xxx = tx * scale + 30;
			yyy = -tz * scale + 30;
			zzz = ty * scale + 21.14868;

			base = min(base, z);
			v.push_back(Vec(x, y, z));
			vv.push_back(Vec(xx, yy, zz));
			vvv.push_back(Vec(xxx, yyy, zzz));
			n.push_back(Vec());

		} else if (buf[0] == 'f') {
			int x, y, z;
			fscanf(fp, "%d %d %d", &x, &y, &z);

			triangles.push_back(Triangle(v[x-1], v[y-1], v[z-1], Vec(0.94,0.94,0.96), METAL));
			idx.push_back(x);
			idy.push_back(y);
			idz.push_back(z);

			triangles.push_back(Triangle(vv[x-1], vv[y-1], vv[z-1], Vec(0.98,0.96,0.73), METAL));
			idx.push_back(x);
			idy.push_back(y);
			idz.push_back(z);

			triangles.push_back(Triangle(vvv[x-1], vvv[y-1], vvv[z-1], Vec(0.78,0.58,0.37), METAL));
			idx.push_back(x);
			idy.push_back(y);
			idz.push_back(z);

			Vec temp_n = ((v[x-1]-v[z-1]) % (v[y-1]-v[z-1])).normal();
			n[x-1] = n[x-1] + temp_n;
			n[y-1] = n[y-1] + temp_n;
			n[z-1] = n[z-1] + temp_n;
		}
	}
	fprintf(stderr, "The base of model is %4f\n",base);
	
	
	for (int i = 0; i < triangles.size(); ++i) {
		triangles[i].np = n[idx[i]-1].normal();
		triangles[i].nq = n[idy[i]-1].normal();
		triangles[i].nr = n[idz[i]-1].normal();
	}
	

	fclose(fp);
	fprintf(stderr, "Finish loading obj from %s\n", file_name);


	// load walls
	real x_wall_min = -1000;
	real x_wall_max = 1000;
	real y_wall_min = -1000;
	real y_wall_max = 1000;
	real z_wall_min = 0;
	real z_wall_max = 50;

	Vec v000(x_wall_min,y_wall_min,z_wall_min);
	Vec v001(x_wall_min,y_wall_min,z_wall_max);
	Vec v010(x_wall_min,y_wall_max,z_wall_min);
	Vec v011(x_wall_min,y_wall_max,z_wall_max);
	Vec v100(x_wall_max,y_wall_min,z_wall_min);
	Vec v101(x_wall_max,y_wall_min,z_wall_max);
	Vec v110(x_wall_max,y_wall_max,z_wall_min);
	Vec v111(x_wall_max,y_wall_max,z_wall_max);

	// down
	triangles.push_back(Triangle(v010,v000,v110,Vec(0.75,0.75,0.75),MARBLE));
	triangles.push_back(Triangle(v110,v000,v100,Vec(0.75,0.75,0.75),MARBLE));

	// left
	// triangles.push_back(Triangle(v001,v000,v011,Vec(0.50,0.25,0.15)));
	// triangles.push_back(Triangle(v011,v000,v010,Vec(0.50,0.25,0.15)));
	// right
	// triangles.push_back(Triangle(v101,v111,v100,Vec(0.99,0.99,0.99)));
	// triangles.push_back(Triangle(v111,v110,v100,Vec(0.99,0.99,0.99)));
	// up
	// triangles.push_back(Triangle(v011,v111,v001,Vec(0.75,0.75,0.75)));
	// triangles.push_back(Triangle(v111,v101,v001,Vec(0.75,0.75,0.75)));
	// back
	// triangles.push_back(Triangle(v011,v010,v111,Vec(0.75,0.75,0.75)));
	// triangles.push_back(Triangle(v111,v010,v110,Vec(0.75,0.75,0.75)));

	// front
	// triangles.push_back(Triangle(v001,v101,v000,Vec(0.25,0.25,0.25)));
	// triangles.push_back(Triangle(v101,v100,v000,Vec(0.25,0.25,0.25)));


	t = new Node[triangles.size() * 4];
	queue = new int[triangles.size() * 4];
	build(1,0,triangles.size()-1);
}