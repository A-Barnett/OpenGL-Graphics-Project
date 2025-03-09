#version 330 core

uniform mat4 modelMatrix; 
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform float time;
uniform float amplitude;
uniform float frequency;
uniform float scrollSpeed;
uniform float waveSpeed;
uniform float amplitudeScale;
uniform vec4 colour;
uniform float posChangeX;
uniform float posChangeZ;

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec4 tangent;

out Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} OUT;

void main(void) {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    vec3 wNormal = normalize(normalMatrix * normalize(normal));
    vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

    OUT.normal = wNormal;
    OUT.tangent = wTangent;
    OUT.binormal = normalize(cross(wTangent, wNormal) * tangent.w);

    vec4 worldPos = modelMatrix * vec4(position, 1);
    worldPos.x+=posChangeX;
    worldPos.z+=posChangeZ;

    float baseY = worldPos.y;
    float waveSum = 0;

    for (int i = 2; i <= 10; i++) {
        float directionX = 1.0 + mod(float(i) * 0.3, 2.0);
        float directionZ = 1.0 + mod(float(i) * 0.7, 3.0);

        waveSum += (amplitude * pow(amplitudeScale, float(i))) * 
                   sin((frequency * i) * (directionX * worldPos.x + directionZ * worldPos.z) + (time * float(i / 5) * waveSpeed));
    }

    worldPos.y = baseY + waveSum;


    OUT.worldPos = worldPos.xyz;
    gl_Position = projMatrix * viewMatrix * worldPos;

    OUT.texCoord = (textureMatrix * vec4(texCoord , 0.0, 1.0)).xy;
    OUT.colour = colour;
}
