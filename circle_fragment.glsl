#version 330 core
out vec4 FragColor;
in vec2 fragCoord;

uniform vec2 circleCenter;
uniform float radius;
uniform int isIntersecting;

void main() {
    float dist = distance(fragCoord, circleCenter);
    if (dist > radius) {
        discard;
    }
    
    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 green = vec3(0.0, 1.0, 0.0);
    
    vec3 centerColor = red;
    vec3 edgeColor = green;
    
    if (isIntersecting == 0) {
        centerColor = green;
        edgeColor = red;
    }
    
    vec3 finalColor = mix(centerColor, edgeColor, dist / radius);
    FragColor = vec4(finalColor, 1.0);
}