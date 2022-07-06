typedef __attribute__ ((packed)) struct {
	float x, y;
} vec2f;

typedef __attribute__ ((packed)) struct {
	float x, y, z;
} vec3f;

typedef __attribute__ ((packed)) struct {
	float x, y, z, w;
} vec4f;

typedef __attribute__ ((packed)) struct {
	float a, b, c,  d, e, f,  g, h, i;
} mat3;


void *mallocArray;
int mallocArrayFst = 0;

void* malloc() {
	return &mallocArray + mallocArrayFst;
}


struct Node {
	struct Node *childs;
	int size;
	char finish;
	char visible;// = 1;
	vec3f color;

	int id;
};

int getSize(int n) {
	int r = 1;
	for (int i = 0; i < n; i++) r *= 2;
	return r;
}

int pow(int a, int b) {
	if (b == 0) return 1;
	for (int i = 0; i < b; i++) a *= a;
	return a;
}

void initNode(struct Node *node, int _size, int _id, char *m) {
	node->size = getSize(_size - 1);
	node->id = _id;
	node->color.x = 1; node->color.y = 1; node->color.z = 1;

	if (_size == 1) node->finish = 1;
	else {
		node->childs = malloc();

		for (int i = 0; i < 8; i++) {
			initNode(&node->childs[i], _size - 1, _id * (int)(pow(8, _size - 1)) + i, m);
		}
	}
}


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

vec3f divVec3fVec3f(vec3f a, vec3f b) {
	vec3f res;

	res.x = a.x / b.x;
	res.y = a.y / b.y;
	res.z = a.z / b.z;

	return res;
}

vec3f divVec3fFloat(vec3f a, float b) {
	vec3f res;

	res.x = a.x / b;
	res.y = a.y / b;
	res.z = a.z / b;

	return res;
}

vec3f sumVec3fVec3f(vec3f a, vec3f b) {
	vec3f res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

vec3f subVec3fVec3f(vec3f a, vec3f b) {
	vec3f res;
	res.x = a.x - b.x;
	res.y = a.y - b.y;
	res.z = a.z - b.z;
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


float length(vec3f v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3f normalize(vec3f v) {
	return divVec3fFloat(v, length(v));
}

float abs(float v) {
	if (v < 0) return -v;
	return v;
}

vec3f vabs(vec3f v) {
	vec3f res;
	res.x = abs(v.x); res.y = abs(v.y); res.z = abs(v.z);
	return res;
}

float dot(vec3f a, vec3f b) {
	return a.x * b.x + a.y * b.y + a.z * b.y;
}

vec2f box(vec3f ro, vec3f rd, vec3f rad) {//, vec3f& oN)  {
	vec3f m; m.x = 1; m.y = 1; m.z = 1;
	vec3f n;
	vec3f k;
	vec3f t1;
	vec3f t2;
	float tN;
	float tF;
	vec2f res; res.x = -1; res.y = -1;

	m = divVec3fVec3f(m, rd);
	n = mulVec3fVec3f(m, ro);
	n.x = -n.x; n.y = -n.y; n.z = -n.z;
	k = mulVec3fVec3f(rad, vabs(m));
	t1 = subVec3fVec3f(n, k);
	t2 = sumVec3fVec3f(n, k);
	tN = max(max(t1.x, t1.y), t1.z);
	tF = min(min(t2.x, t2.y), t2.z);

	if(tN > tF || tF < 0.0) return res;

	res.x = tN; res.y = tF;
	return res;
	//oN = (vec3f(0)-sign(rd)) * step(vec3f(t1.y, t1.z, t1.x), t1) * step(vec3f(t1.z, t1.x, t1.y), t1);
}

kernel void kmain(
	global vec3f *ro, global mat3 *rot,
	global vec3f *output, global int *_width, global int *_height,
	global char *m,
	local char* cache) {

	//mallocArray = cache;

	//struct Node node;

	//initNode(&node, 3, 0, m);

	int width = *_width;
	int height = *_height;
	int id = get_global_id(0);
	int x = id % width;
	int y = id / width;

	float u = (float)x / width  * 2. - 1.;
	float v = (float)y / height * 2. - 1.;

	u *= (float)width / height;

	vec3f rd; rd.x = u; rd.y = v; rd.z = 1;
	rd = mulVec3fMat3(normalize(rd), *rot);

	vec3f col = {0,0,0};
	//col = render(*ro, rd, oct, 1);
	output[id] = col;
}
