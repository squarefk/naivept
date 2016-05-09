#include "model.h"

#include "vec.h"
#include "ray.h"
#include "triangle.h"
#include <cmath>
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
	float lenx = t[k].high.x - t[k].low.x;
	float leny = t[k].high.y - t[k].low.y;
	float lenz = t[k].high.z - t[k].low.z;
	sort(triangles.begin()+l,triangles.begin()+r+1, (lenx>leny && lenx>lenz)?cmpx:(leny>lenz?cmpy:cmpz));
	if (l == r) return;
	build(k+k,l,t[k].m);
	build(k+k+1,t[k].m+1,r);
}

bool Model::intersect(const Ray& ray, float& dist, Vec& n, Vec& c, Material& m) {
	bool found = false;
	queue[1]=1;
	for (int l = 1, r = 1; l <= r; ++l) {
		int k = queue[l];
		if (t[k].l==t[k].r) {
			float d;
			bool ret = ray.intersect_with_triangle(triangles[t[k].l], d);
			if (ret && d>1e-4 && (!found || d < dist)) {
				found = true;
				dist = d;
				n = triangles[t[k].l].n;
				c = triangles[t[k].l].c;
				m = triangles[t[k].l].m;
			}
			continue;
		}
		if (!ray.intersect_with_box(t[k].low, t[k].high)) continue;
		queue[++r] = k+k;
		queue[++r] = k+k+1;
	}

	for (int i = 0; i < walls.size(); ++i) {
		float d;
		bool ret = ray.intersect_with_triangle(walls[i], d);
		if (ret && d>1e-4 && (!found || d < dist)) {
			found = true;
			dist = d;
			n = walls[i].n;
			c = walls[i].c;
			m = walls[i].m;
		}
	}

	return found;
}


void Model::load_from_obj(const char* file_name) {
	FILE* fp =fopen(file_name, "r");
	printf("Start loading obj from %s\n", file_name);

	char buf[256];
	int lineNumber = 0;
	vector<Vec> v;
	while (fscanf(fp, "%s", buf) != EOF) {
		if (buf[0] == '#') {
			fgets(buf, sizeof(buf), fp);
		} else if (buf[0] == 'v') {
			float x, y, z;
			fscanf(fp, "%f %f %f", &x, &y, &z);
			x = x * 160 - 80;
			y = y * 160 - 80;
			z = z * 2 + 10;
			v.push_back(Vec(x, y, z));
		} else {
			// buf[0] == 'f'
			int x, y, z;
			fscanf(fp, "%d %d %d", &x, &y, &z);
			triangles.push_back(Triangle(v[x-1], v[y-1], v[z-1], Vec(0.9,0.9,0.9), REFR));
		}
	}

	fclose(fp);
	printf("Finish loading obj from %s\n", file_name);


	// load walls
	float x_wall_min = -76.5;
	float x_wall_max = 76.5;
	float y_wall_min = -76.5;
	float y_wall_max = 76.5;
	float z_wall_min = -16.5;
	float z_wall_max = 86.5;

	Vec v000(x_wall_min,y_wall_min,z_wall_min);
	Vec v001(x_wall_min,y_wall_min,z_wall_max);
	Vec v010(x_wall_min,y_wall_max,z_wall_min);
	Vec v011(x_wall_min,y_wall_max,z_wall_max);
	Vec v100(x_wall_max,y_wall_min,z_wall_min);
	Vec v101(x_wall_max,y_wall_min,z_wall_max);
	Vec v110(x_wall_max,y_wall_max,z_wall_min);
	Vec v111(x_wall_max,y_wall_max,z_wall_max);
	// down
	walls.push_back(Triangle(v000,v010,v100,Vec(0.75,0.75,0.75)));
	walls.push_back(Triangle(v110,v010,v100,Vec(0.75,0.75,0.75)));
	// left
	walls.push_back(Triangle(v000,v010,v001,Vec(0.75,0.25,0.25)));
	walls.push_back(Triangle(v001,v010,v011,Vec(0.75,0.25,0.25)));
	// right
	walls.push_back(Triangle(v110,v100,v101,Vec(0.25,0.25,0.75)));
	walls.push_back(Triangle(v111,v101,v110,Vec(0.25,0.25,0.75)));
	// up
	walls.push_back(Triangle(v001,v111,v101,Vec(0.75,0.75,0.75)));
	walls.push_back(Triangle(v011,v001,v111,Vec(0.75,0.75,0.75)));
	// back
	walls.push_back(Triangle(v000,v100,v101,Vec(0.75,0.75,0.75)));
	walls.push_back(Triangle(v000,v101,v001,Vec(0.75,0.75,0.75)));


	t = new Node[triangles.size() * 4];
	queue = new int[triangles.size() * 4];
	build(1,0,triangles.size()-1);
}