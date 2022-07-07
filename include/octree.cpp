#ifndef _OCTREE
#define _OCTREE


#include <vector.cpp>


vec2f box(vec3f ro, vec3f rd, vec3f rad) { //, vec3f& oN)  {
	vec3f m = vec3f(1.0) / rd;
	vec3f n = m * ro;
	vec3f k = rad * abs(m);
	vec3f t1 = -n - k;
	vec3f t2 = vec3f(0)-n + k;
	float tN = fmax(fmax(t1.x, t1.y), t1.z);
	float tF = fmin(fmin(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec2f(-1.0);
	//oN = (vec3f(0)-sign(rd)) * step(vec3f(t1.y, t1.z, t1.x), t1) * step(vec3f(t1.z, t1.x, t1.y), t1);
	return vec2f(tN, tF);
}


int getSize(int n) {
	int r = 1;

	for (int i = 0; i < n - 1; i++) r *= 2;

	return r;
}


struct Node {
	Node* childs;
	int size;
	char finish;
	char visible = 1;
	vec3<unsigned char> color;

	int id;

	Node(){}
	Node (int _size, int _id, char *m) {init(_size, _id, m);}

	void init(int _size, int _id, char *m) {
		id = _id;

		size = getSize(_size);
		color = vec3<unsigned char>(
			rand() & 255,
			rand() & 255,
			rand() & 255
		);

		if (_size == 1) {
			finish = 1;
			visible = m[id];
			//printf("%i ", id);
		} else {
			childs = new Node[8];
			for (int i = 0; i < 8; i++)
				childs[i].init(_size - 1, _id * (int)(pow(8, _size - 1)) + i, m);
		}
	}

	Node* get(int id) {
		return &(childs[id]);
	}

	char getVisible() {
		if (size == 1) return visible;

		visible = 0;

		for (int i = 0; i < 8; i++) {
			if (childs[i].getVisible() == 1) {
				visible = 1;
			}
		}

		if (visible == 0 || size == 1) finish = 1;
		else finish = 0;

		//printf("%i\n", visible);

		return visible;
	}

	vec4<unsigned char> trayce(vec3f ro, vec3f rd, vec2f &pit, int d) {
		if (visible == 0)
			return vec4<unsigned char>(0);

		vec2f it = box(ro, rd, vec3f(size));
		if (it.x < 0 || pit.x < it.x) {
			//if (visible == 0)
				return vec4<unsigned char>(0);
		}

		if (d == 1 || finish == 1) {
			pit = it;
			return vec4<unsigned char>(color.x,color.y,color.z, 1);
		}

		vec4<unsigned char> res;
		vec2f nit(it.y + 1);

		for (int i = 0; i < 8; i++) {
			float x = (float)(i % 2)     - 0.5;
			float y = (float)(i % 4 / 2) - 0.5;
			float z = (float)(i / 4)     - 0.5;

			vec4<unsigned char> c = childs[i].trayce(ro - vec3f(x,y,z) * size, rd, nit, d - 1);

			if (c.w == 1) res = c;
		}

		pit = nit;

		return res;
	}
};


#endif
