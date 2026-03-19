#version 330 core
layout (location = 0) in vec2 aPos;

void main() {
    vec2 ndcPos = (aPos / 300.0) - 1.0;
    gl_Position = vec4(ndcPos, 0.0, 1.0);
}