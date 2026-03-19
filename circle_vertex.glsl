#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 circleCenter;
out vec2 fragCoord;

void main() {
    vec2 pos = aPos + circleCenter;
    fragCoord = pos;
    vec2 ndcPos = (pos / 300.0) - 1.0;
    gl_Position = vec4(ndcPos, 0.0, 1.0);
}