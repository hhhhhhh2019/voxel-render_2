#version 410 core

in vec4 gl_FragCoord;
//out vec4 gl_FragColor;


uniform vec2 resolution;
uniform vec3 cam_pos;
uniform mat3 cam_rot;
uniform int model_size;
uniform vec4 model[512];


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


vec4 trayce12(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);

	pMinIt = it;

	return vec4(abs(n), 1);
}

vec4 trayce11(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce12(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce10(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce11(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce9(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce10(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce8(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce9(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce7(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce8(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce6(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce7(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce5(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce6(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce4(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce5(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce3(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce4(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce2(vec3 ro, vec3 rd, int size, inout vec2 pMinIt, int id, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4 || pMinIt.x < it.x) return vec4(0);
	if (size == 1) {
		if (model[id].w == 0) return vec4(0);
		pMinIt = it;
		return model[id];
	}

	vec4 res;
	vec2 minIt = pMinIt;//vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce3(ro - vec3(x, y, z) * size, rd, size / 2, minIt, id + i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	if (res.w != 0)
		pMinIt = minIt;

	return res;
}

vec4 trayce1(vec3 ro, vec3 rd, int size, int s) {
	vec3 n;
	vec2 it = box(ro, rd, vec3(size), n);

	if (it.x == -1 || it.x <= -size * 4) return vec4(0);
	if (size == 1) {
		return model[0];
	}

	vec4 res = vec4(0.1,0.1,0.1,1);
	vec2 minIt = vec2(it.y);

	for (int i = 0; i < voxInNode; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce2(ro - vec3(x, y, z) * size, rd, size / 2, minIt, i * ipow(8, s - 1), s - 1);

		if (col.w != 0) res = col;
	}

	return res;
}


void main() {
	vec2 uv = (gl_FragCoord.xy / resolution - 0.5) * resolution / resolution.y;

	vec3 rd = normalize(vec3(uv,1)) * cam_rot;

	vec3 color = trayce1(cam_pos, rd, getSize(model_size), model_size / 2).rgb;

	gl_FragColor = vec4(color,1);
}
