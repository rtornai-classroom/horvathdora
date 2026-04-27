#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3  lightPosition;
uniform vec3  lightColor;
uniform bool  isLightOn;
uniform bool  isSun;
uniform sampler2D textureSampler;

out vec4 FragColor;

void main() {
    if (isSun) {
        // A Nap megvilágítás nélkül is világít (Emission)
        vec4 texColor = texture(textureSampler, TexCoords);
        FragColor = texColor; 
    } else {
        // Kockák (fehér színűek)
        vec3 objectColor = vec3(1.0, 1.0, 1.0);
        
        // Alapfény (Ambient), hogy teljes sötétben is látszódjon a sziluett
        float ambientStrength = 0.15;
        vec3 ambient = ambientStrength * objectColor;
        
        vec3 result = ambient;
        
        // Diffúz fény hozzáadása, ha ég a villany
        if (isLightOn) {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPosition - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * objectColor;
            
            result += diffuse;
        }
        
        FragColor = vec4(result, 1.0);
    }
}