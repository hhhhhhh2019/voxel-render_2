#version 410 core

in vec4 gl_FragCoord;
//out vec4 gl_FragColor;


uniform vec2 resolution;
uniform vec3 cam_pos;
uniform mat3 cam_rot;
uniform int model_size;
uniform vec4 model[512];

uniform sampler2D backBuffer;
uniform bool useBackBuffer;


#define voxInNode 8


vec2 box(in vec3 ro, in vec3 rd, in vec3 rad, out vec3 oN)  {
	vec3 m = 1.0 / rd;
	vec3 n = m * ro;
	vec3 k = abs(m) * rad;
	vec3 t1 = -n - k;
	vec3 t2 = -n + k;
	float tN = max(max(t1.x, t1.y), t1.z);
	float tF = min(min(t2.x, t2.y), t2.z);
	if(tN > tF || tF < 0.0) return vec2(-1);
	oN = -sign(rd) * step(t1.yzx, t1.xyz) * step(t1.zxy, t1.xyz);
	return vec2(tN, tF);
}

int getSize(int s) {
	int r = 1;

	for (int i = 0; i < s - 1; i++) r *= 2;

	return r;
}

int ipow(int a, int n) {
	if (n == 0) return 1;
	for (int i = 0; i < n; i++) a *= a;
	return a;
}



const int MAX_REFL = 6;

float seed;

uniform vec2 u_seed1;
uniform vec2 u_seed2;

uvec4 R_STATE;

uint TausStep(uint z, int S1, int S2, int S3, uint M)
{
	uint b = (((z << S1) ^ z) >> S2);
	return (((z & M) << S3) ^ b);
}

uint LCGStep(uint z, uint A, uint C)
{
	return (A * z + C);
}

vec2 hash22(vec2 p)
{
	p += u_seed1.x;
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx+33.33);
	return fract((p3.xx+p3.yz)*p3.zy);
}

float random()
{
	R_STATE.x = TausStep(R_STATE.x, 13, 19, 12, uint(429496729));
	R_STATE.y = TausStep(R_STATE.y, 2, 25, 4, uint(429496728));
	R_STATE.z = TausStep(R_STATE.z, 3, 11, 17, uint(429496728));
	R_STATE.w = LCGStep(R_STATE.w, uint(1664525), uint(101390422));
	return 2.3283064365387e-10 * float((R_STATE.x ^ R_STATE.y ^ R_STATE.z ^ R_STATE.w));
}

float rand(){
	float r = fract(sin(dot(vec2(seed),vec2(12.9898,78.233))) * 43758.5453);
	seed = r;
	return r;
}

vec3 randomOnSphere() {
	//return normalize(vec3(rand()*2.-1., rand()*2.-1., rand()*2.-1.));
	//return normalize(vec3(random()*2.-1., random()*2.-1., random()*2.-1.));

	vec3 rand = vec3(random(), random(), random());
	float theta = rand.x * 2.0 * 3.14159265;
	float v = rand.y;
	float phi = acos(2.0 * v - 1.0);
	float r = pow(rand.z, 1.0 / 3.0);
	float x = r * sin(phi) * cos(theta);
	float y = r * sin(phi) * sin(theta);
	float z = r * cos(phi);
	return vec3(x, y, z);
}


vec4 trayce12(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);

	pMinIt = it;

	pn = n;

	return vec4(abs(n), 1);
}

vec4 trayce11(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce12(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce10(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce11(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce9(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce10(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce8(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce9(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce7(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce8(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce6(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce7(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce5(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce6(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce4(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce5(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce3(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce4(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	pn = n;

	return res;
}

vec4 trayce2(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s, vec3 pn) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		pn = n;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce3(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w != 0) {
		pMinIt = minIt;
		pn = n;
	}

	return res;
}

vec4 trayce1(inout vec3 ro, inout vec3 rd, int size, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4) return vec4(0);
	if (size == 1) {
		return model[0];
	}

	vec4 res;
	vec2 minIt = vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce2(ro - vec3(x, y, z) * size, rd, size / 2, minIt, i * ipow(8, s - 1), s - 1, n);

		if (col.w != 0) res = col;
	}

	if (res.w == -2) return res;
	if (res.w == 0) return vec4(0);

	vec3 reflected = reflect(rd, n);
	vec3 diff      = normalize(randomOnSphere());
	vec3 diffuse   = normalize(diff * dot(diff, n));

	ro += rd * (minIt.x - 0.001);
	rd = mix(diffuse, reflected, res.w - 1);

	return res;
}

vec3 render(vec3 ro, vec3 rd) {
	vec3 col = vec3(1);

	for (int i = 0; i < MAX_REFL; i++) {
		vec4 nc = trayce1(ro, rd, getSize(model_size), model_size / 2);//trayce(ro, rd, getSize(size),size,size-1);

		col *= nc.rgb;

		if (nc.a == -2.) return col;
	}

	return vec3(0);
}

void main() {
	vec2 uv = (gl_FragCoord.xy / resolution - 0.5) * resolution / resolution.y;

	vec2 uvRes = hash22(uv + 1.0) * resolution + resolution;
	R_STATE.x = uint(u_seed1.x + uvRes.x);
	R_STATE.y = uint(u_seed1.y + uvRes.x);
	R_STATE.z = uint(u_seed2.x + uvRes.y);
	R_STATE.w = uint(u_seed2.y + uvRes.y);

	vec3 rd = normalize(vec3(uv,1)) * cam_rot;

	vec3 color = vec3(0);
	int samples = 4;
	for(int i = 0; i < samples; i++) {
		color += render(cam_pos, rd);
	}
	color /= samples;

	vec2 iuv = gl_FragCoord.xy / resolution;

	if (useBackBuffer == true)
		color = mix(sqrt(color), texture(backBuffer, iuv).rgb, 0.95);
	else
		color = sqrt(color);

	gl_FragColor = vec4(color,1);
}
