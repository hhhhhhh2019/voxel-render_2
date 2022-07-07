#version 410

uniform vec2 resolution;
uniform vec3 ro;
uniform mat3 rot;
uniform int size;
uniform vec4 model[512];

in vec4 gl_FragCoord;
out vec4 fragColor;

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


vec4 trayce7(vec3 ro, vec3 rd, int s, int d, inout vec2 pit, int id, int _s, out vec3 norm) {
	vec3 n;
	vec2 it = box(ro,rd,vec3(s),n);

	if (it.x < -s * 2 || it.x == -1 || pit.x < it.x) return vec4(0);

	pit = it;
	norm = n;

	return model[id];
	/*if (d == 1 || s == 1) return vec4(1);

	vec2 nit = vec2(it.y + 1);
	vec4 res = vec4(0);

	for (int i = 0; i < 8; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce3(ro - vec3(x,y,z) * s, rd, s / 2, d - 1, nit);

		if (col.w == 1) res = col;
	}

	pit = nit;

	return res;*/
}


vec4 trayce6(vec3 ro, vec3 rd, int s, int d, inout vec2 pit, int id, int _s, out vec3 norm) {
	vec3 n;
	vec2 it = box(ro,rd,vec3(s),n);

	if (it.x < -s * 2 || it.x == -1 || pit.x < it.x) return vec4(0);
	if (d == 1 || s == 1) {
		if (model[id].w == 0) return vec4(0);
		pit = it;
		norm = n;
		return model[id];
	}

	vec2 nit = vec2(pit.y + 1);
	vec4 res = vec4(0);

	for (int i = 0; i < 8; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce7(ro - vec3(x,y,z) * s, rd, s / 2, d - 1, nit, int(id * pow(8, _s - 1) + i), _s - 1, norm);

		if (col.w == 1) res = col;
	}

	pit = nit;

	return res;
}


vec4 trayce5(vec3 ro, vec3 rd, int s, int d, inout vec2 pit, int id, int _s, out vec3 norm) {
	vec3 n;
	vec2 it = box(ro,rd,vec3(s),n);

	if (it.x < -s * 2 || it.x == -1 || pit.x < it.x) return vec4(0);
	if (d == 1 || s == 1) {
		if (model[id].w == 0) return vec4(0);
		pit = it;
		norm = n;
		return model[id];
	}

	vec2 nit = vec2(pit.y + 1);
	vec4 res = vec4(0);

	for (int i = 0; i < 8; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce6(ro - vec3(x,y,z) * s, rd, s / 2, d - 1, nit, int(id * pow(8, _s - 1) + i), _s - 1, norm);

		if (col.w == 1) res = col;
	}

	pit = nit;

	return res;
}


vec4 trayce4(vec3 ro, vec3 rd, int s, int d, inout vec2 pit, int id, int _s, out vec3 norm) {
	vec3 n;
	vec2 it = box(ro,rd,vec3(s),n);

	if (it.x < -s * 2 || it.x == -1 || pit.x < it.x) return vec4(0);
	if (d == 1 || s == 1) {
		if (model[id].w == 0) return vec4(0);
		pit = it;
		norm = n;
		return model[id];
	}

	vec2 nit = vec2(pit.y + 1);
	vec4 res = vec4(0);

	for (int i = 0; i < 8; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce5(ro - vec3(x,y,z) * s, rd, s / 2, d - 1, nit, int(id * pow(8, _s - 1) + i), _s - 1, norm);

		if (col.w == 1) res = col;
	}

	pit = nit;

	return res;
}


vec4 trayce3(vec3 ro, vec3 rd, int s, int d, inout vec2 pit, int id, int _s, out vec3 norm) {
	vec3 n;
	vec2 it = box(ro,rd,vec3(s),n);

	if (it.x < -s * 2 || it.x == -1 || pit.x < it.x) return vec4(0);
	if (d == 1 || s == 1) {
		if (model[id].w == 0) return vec4(0);
		pit = it;
		norm = n;
		return model[id];
	}

	vec2 nit = vec2(pit.y + 1);
	vec4 res = vec4(0);

	for (int i = 0; i < 8; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce4(ro - vec3(x,y,z) * s, rd, s / 2, d - 1, nit, int(id * pow(8, _s - 1) + i), _s - 1, norm);

		if (col.w == 1) res = col;
	}

	pit = nit;

	return res;
}


vec4 trayce2(vec3 ro, vec3 rd, int s, int d, inout vec2 pit, int id, int _s, out vec3 norm) {
	vec3 n;
	vec2 it = box(ro,rd,vec3(s),n);

	if (it.x < -s * 2 || it.x == -1 || pit.x < it.x) return vec4(0);
	if (d == 1 || s == 1) {
		if (model[id].w == 0) return vec4(0);
		pit = it;
		norm = n;
		return model[id];
	}

	vec2 nit = vec2(pit.y + 1);
	vec4 res = vec4(0);

	for (int i = 0; i < 8; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce3(ro - vec3(x,y,z) * s, rd, s / 2, d - 1, nit, int(id * pow(8, _s - 1) + i), _s - 1, norm);

		if (col.w == 1) res = col;
	}

	pit = nit;

	return res;
}

vec4 trayce(vec3 ro, vec3 rd, int s, int d, int _s, out vec3 norm) {
	vec3 n;
	vec2 it = box(ro,rd,vec3(s),n);

	if (it.x < -s * 2 || it.x == -1) return vec4(0);
	if (d == 1 || s == 1) {
		norm = n;
		return model[0];
	}

	vec2 nit = vec2(it.y + 1);
	vec4 res = vec4(0);

	for (int i = 0; i < 8; i++) {
		float x = float(i % 2)     - 0.5;
		float y = float(i % 4 / 2) - 0.5;
		float z = float(i / 4)     - 0.5;

		vec4 col = trayce2(ro - vec3(x,y,z) * s, rd, s / 2, d - 1, nit, i, _s - 1, norm);

		if (col.w == 1) res = col;
	}

	return res;
}


int getSize(int n) {
	int r = 1;

	for (int i = 0; i < n - 1; i++) r *= 2;

	return r;
}


void main() {
	vec2 uv = (gl_FragCoord.xy / resolution - 0.5) * resolution / resolution.y;

	vec3 rd = normalize(vec3(uv,1)) * rot;

	vec3 norm;

	vec4 col = trayce(ro,rd,getSize(size),size,size,norm);

	col *= dot(norm, -rd) * 0.5 + 0.5;

	fragColor = vec4(col.rgb,1);
}
