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
	double lenx = t[k].high.x - t[k].low.x;
	double leny = t[k].high.y - t[k].low.y;
	double lenz = t[k].high.z - t[k].low.z;
	sort(triangles.begin()+l,triangles.begin()+r+1, (lenx>leny && lenx>lenz)?cmpx:(leny>lenz?cmpy:cmpz));
	if (l == r) return;
	build(k+k,l,t[k].m);
	build(k+k+1,t[k].m+1,r);
}

void Model::detect(int k, const Ray& ray) {
	if (t[k].l==t[k].r) {
		double d;
		bool ret = ray.intersect_with_triangle(triangles[t[k].l], d);
		if (ret && d>1e-4 && (!_found || d < _dist)) {
			_found = true;
			_dist = d;
			_n = ((triangles[t[k].l].q - triangles[t[k].l].p) %
				(triangles[t[k].l].r - triangles[t[k].l].p)).normal();
		}
		return;
	}
	if (!ray.intersect_with_box(t[k].low, t[k].high)) return;
	detect(k+k, ray);
	detect(k+k+1, ray);
}


bool Model::intersect(const Ray& ray, double& dist, Vec& n) {
	_found = false;
	detect(1, ray);
	dist = _dist;
	n = _n;
	return _found;
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
			double x, y, z;
			fscanf(fp, "%lf %lf %lf", &x, &y, &z);
			double yy = -z;
			double zz = y;
			z = zz;
			y = yy;
			x = - x * 30 + 30;
			y = y * 30 + 20;
			z = z * 30 + 13.5;
			v.push_back(Vec(x, y, z));
		} else {
			// buf[0] == 'f'
			int x, y, z;
			fscanf(fp, "%d %d %d", &x, &y, &z);
			triangles.push_back(Triangle(v[x-1], v[y-1], v[z-1]));
		}
	}

	fclose(fp);
	printf("Finish loading obj from %s\n", file_name);

	t = new Node[triangles.size() * 4];
	build(1,0,triangles.size()-1);
}