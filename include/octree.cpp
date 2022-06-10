#pragma once

#include <vector.cpp>


int degree(int x) {
	int r = 2;
	while (x != r) {
		if ((float)x / r > 1.0f) r *= 2;
		else x++;
	}
	return r;
}


vec2f box(vec3f ro, vec3f rd, vec3f rad) {//, vec3f& oN)  {
	vec3f m = vec3f(1.0) / rd;
	vec3f n = m * ro;
	vec3f k = rad * abs(m);
	vec3f t1 = vec3f(0)-n - k;
	vec3f t2 = vec3f(0)-n + k;
	float tN = fmax(fmax(t1.x, t1.y), t1.z);
	float tF = fmin(fmin(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec2f(-1.0);
	//oN = (vec3f(0)-sign(rd)) * step(vec3f(t1.y, t1.z, t1.x), t1) * step(vec3f(t1.z, t1.x, t1.y), t1);
	return vec2f(tN, tF);
}


struct Node {
private:
	Node *nodes;
	bool final = true;
	unsigned int size;

public:
	vec3f color = vec3f(0);
	bool visible = false;

	Node() {} //{color.x = rand() & 255;visible = rand() & 1;}
	~Node() {
		if (final == true) return;
		delete[] nodes;
	}

	bool getVisible() {
		if (final) return visible;
		bool v = false;
		vec3f col;
		int c = 0;
		for (int i = 0; i < 8; i++) {
			bool nv = nodes[i].getVisible();
			if (nv == true) c++;
			v |= nv;
			col.x += nodes[i].color.x;
			col.y += nodes[i].color.y;
			col.z += nodes[i].color.z;
		}
		visible = v;
		vec3f nc = col / (float)c;
		color.x = nc.x; color.y = nc.y; color.z = nc.z;
		return visible;
	}

	void set_size(unsigned int s) {
		size = s;
		if (s == 1) final = true;
	}

	bool isFinal() {return final;}
	void setFinal(bool v) {
		if (final == v || size == 1) return;
		final = v;
		if (v == false) {
			nodes = new Node[8];
			for (int i = 0; i < 8; i++) nodes[i].set_size(size / 2);
		}
		else delete[] nodes;
	}

	Node* get(int id) {return &nodes[id];}
	Node* operator[](int id) {return get(id);}

	vec4f trayce(vec3f ro, vec3f rd, vec2f &pit, int d) {
		if (visible == false) return vec4f(0);
		vec2f it = box(ro, rd, vec3f(size));
		if (it.x == -1) return vec4f(0);
		pit = it;
		if (size == 1 || d == 1 || final)
			return vec4f(color.x, color.y, color.z, 1);

		vec2f minIt(it.y);
		vec4f res;

		for (int i = 0; i < 8; i++) {
			float x = (float)(i % 2)     - 0.5;
			float y = (float)(i % 4 / 2) - 0.5;
			float z = (float)(i / 4)     - 0.5;

			vec4f col = nodes[i].trayce(ro - vec3(x,y,z) * size, rd, it, d - 1);
			if (col.w != 0 && it.x < minIt.x) {
				res = col;
				minIt = it;
			}
		}

		return res;
	}
};


struct Octree {
private:
	Node *nodes;
	unsigned int size;

public:
	Octree(unsigned int s): size(s) {
		if (s < 2) {
			printf("size must be >= 2\n");
			delete this;
			return;
		}

		nodes = new Node[8];

		for (int i = 0; i < 8; i++) nodes[i].set_size(size / 2);
	}

	void getVisible() {
		for (int i = 0; i < 8; i++) nodes[i].getVisible();
	}

	Node* get(int id) {return &nodes[id];}
	Node* operator[](int id) {return get(id);}

	vec4f trayce(vec3f ro, vec3f rd, int d) {
		vec2f it = box(ro, rd, vec3f(size));
		if (it.x == -1) return vec4f(0);
		if (d == 1) return vec4f(255,255,255,1);

		vec2f minIt(it.y);
		vec4f res;

		for (int i = 0; i < 8; i++) {
			float x = (float)(i % 2)     - 0.5;
			float y = (float)(i % 4 / 2) - 0.5;
			float z = (float)(i / 4)     - 0.5;

			vec4f col = nodes[i].trayce(ro - vec3(x,y,z) * size, rd, it, d - 1);
			if (col.w != 0 && it.x < minIt.x) {
				res = col;
				minIt = it;
			}
		}

		return res;
	}
};


int max(int a, int b) {return a < b ? b : a;}


Octree* makeOctreeFromArray(vec4f *arr, int w, int h, int d) {
	int s = max(max(degree(w), degree(h)), degree(d));
	Octree* res = new Octree(s);

	for (int i = 0; i < w * h * d; i++) {
		int x = i % w;
		int y = i % (w * h) / w;
		int z = i / w / h;

		int id = x + y * s + z * s * s;

		res->get(id)->color = vec3f(arr[i].x,arr[i].y,arr[i].z);
		res->get(id)->visible = arr[i].w;

		printf("%i %i %i %i\n", x, y, z, id);
	}

	return res;
}
