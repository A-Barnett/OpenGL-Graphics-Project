#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

// Instance-specific attributes for each tile
layout(location = 2) in float tessInner;
layout(location = 3) in float tessOuterLeft;
layout(location = 4) in float tessOuterRight;
layout(location = 5) in float tessOuterTop;
layout(location = 6) in float tessOuterBottom;
layout(location = 7) in float posChangeX;
layout(location = 8) in float posChangeZ;

out VertexData {
   vec4 colour;
   vec2 texCoord;
   vec3 worldPos;
   float tessInner;
   float tessOuterLeft;
   float tessOuterRight;
   float tessOuterTop;
   float tessOuterBottom;
} outData;

uniform mat4 modelMatrix; 
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform vec4 colour;

void main(void) {
    vec4 worldPos = modelMatrix * vec4(position, 1);
    worldPos.x += posChangeX; // Apply per-instance position offset
    worldPos.z += posChangeZ;

    outData.worldPos = worldPos.xyz;
    gl_Position = projMatrix * viewMatrix * worldPos;

    outData.texCoord = texCoord;
    outData.colour = colour;
    outData.tessInner=tessInner;
    outData.tessOuterLeft=tessOuterLeft;
    outData.tessOuterRight=tessOuterRight;
    outData.tessOuterTop=tessOuterTop;
    outData.tessOuterBottom=tessOuterBottom;
}
