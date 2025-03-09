#version 430 core

in VertexData {
    vec4 colour;
} inData;

out vec4 fragColour;


void main() {
    fragColour = inData.colour;
}