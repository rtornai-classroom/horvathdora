#version 400
out vec4 outColor;
uniform vec3 uColor; // Szín átvétele a C++ kódból
void main() {
	outColor = vec4(uColor, 1.0);
}