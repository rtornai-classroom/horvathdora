#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat3 inverseTransposeMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    FragPos = vec3(matModel * vec4(aPosition, 1.0));
    Normal = inverseTransposeMatrix * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = matProjection * matView * vec4(FragPos, 1.0);
}