#include <cstdio>
using namespace std;

int main() {
	freopen("angel.obj","w",stdout);
	FILE* fp =fopen("Winged_Victory.obj", "r");
	char buf[256];
	while (fscanf(fp, "%s", buf) != EOF) {
		if (buf[0] == '#') {
			fgets(buf, sizeof(buf), fp);
		} else if (buf[0] == 'v' && buf[1] != 't') {
			double x, y, z;
			fscanf(fp, "%lf %lf %lf", &x, &y, &z);
			printf("v %.10f %.10f %.10f\n", x, y, z);
		} else if (buf[0] == 'f') {
			int x,y,z,w,xx,yy,zz,ww;
			fscanf(fp, "%d/%d %d/%d %d/%d %d/%d", &x,&xx,&y,&yy,&z,&zz,&w,&ww);
			printf("f %d %d %d\n",x,y,z);
			printf("f %d %d %d\n",x,z,w);
		}
	}
	return 0;
}
	