#version 440 core

uniform sampler2D t_output;
uniform vec2 resolution;

out vec4 fragColor;

void main() {
	fragColor = vec4(texture(t_output, gl_FragCoord.xy / resolution).rgb,1);
}
