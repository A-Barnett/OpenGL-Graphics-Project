#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;
uniform vec3 startingPos;

in vec3 position;
in vec2 texCoord;

out Vertex {
    vec2 texCoord;
    vec4 colour;
} OUT;

void main(void) {
    vec3 worldPos = position;
    worldPos+=startingPos;

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(worldPos, 1.0);
    OUT.texCoord = texCoord;
    OUT.colour = nodeColour;
}
