typedef __attribute__ ((packed)) struct {
	float x, y, z;
} vec3f;

typedef __attribute__ ((packed)) struct {
	float a, b, c,  d, e, f,  g, h, i;
} mat3;


struct Node {
	struct Node *nodes;
	bool final;
	unsigned int size;

	vec3f color;
	bool visible;
};

struct Octree {
	struct Node *nodes;
	unsigned int size;
};


mat3 mulMat3Mat3(mat3 a, mat3 b) {
	mat3 res;

	res.a = a.a*b.a + a.b*b.d + a.c*b.g;
	res.b = a.a*b.b + a.b*b.e + a.c*b.h;
	res.c = a.a*b.c + a.b*b.f + a.c*b.i;

	res.d = a.d*b.a + a.e*b.d + a.f*b.g;
	res.e = a.d*b.b + a.e*b.e + a.f*b.h;
	res.f = a.d*b.c + a.e*b.f + a.f*b.i;

	res.g = a.g*b.a + a.h*b.d + a.i*b.g;
	res.h = a.g*b.b + a.h*b.e + a.i*b.h;
	res.i = a.g*b.c + a.h*b.f + a.i*b.i;

	return res;
}

vec3f mulVec3fMat3(vec3f v, mat3 m) {
	vec3f res;

	res.x = v.x * m.a + v.y * m.b + v.z * m.c;
	res.y = v.x * m.d + v.y * m.e + v.z * m.f;
	res.z = v.x * m.g + v.y * m.h + v.z * m.i;

	return res;
}

vec3f mulVec3fVec3f(vec3f a, vec3f b) {
	vec3f res;

	res.x = a.x * b.x;
	res.y = a.y * b.y;
	res.z = a.z * b.z;

	return res;
}

vec3f mulVec3fFloat(vec3f a, float b) {
	vec3f res;

	res.x = a.x * b;
	res.y = a.y * b;
	res.z = a.z * b;

	return res;
}

mat3 rotMat3FromVec3f(vec3f v) {
	mat3 a;
	mat3 b;
	mat3 c;

	a.a = 1; a.b = 0;        a.c = 0;
	a.d = 0; a.e = cos(v.x); a.f = -sin(v.x);
	a.g = 0; a.h = sin(v.x); a.i = cos(v.x);

	b.a = cos(v.y); b.b = 0; b.c = -sin(v.y);
	b.d = 0;        b.e = 1; b.f = 0;
	b.g = sin(v.y); b.h = 0; b.i = cos(v.y);

	c.a = cos(v.z); c.b = -sin(v.z); c.c = 0;
	c.d = sin(v.z); c.e = cos(v.z);  c.f = 0;
	c.g = 0;        c.h = 0;         c.i = 1;

	return mulMat3Mat3(mulMat3Mat3(c,b),a);
}


kernel void kmain(global vec3f *ro, global vec3f *rot, global vec3f *output) {

}
