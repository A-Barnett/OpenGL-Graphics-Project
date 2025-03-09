#version 430 core
layout(vertices = 4) out;

// Tessellation levels from instance data
//in float tessInner[];
//in float tessOuterLeft[];
//in float tessOuterRight[];
//in float tessOuterTop[];
//in float tessOuterBottom[];

in VertexData {
    vec4 colour;
    vec2 texCoord;
    vec3 worldPos;
    float tessInner;
    float tessOuterLeft;
    float tessOuterRight;
    float tessOuterTop;
    float tessOuterBottom;
} inData[];

out VertexData {
    vec4 colour;
    vec2 texCoord;
    vec3 worldPos;
} outData[];

void main() {
    gl_TessLevelOuter[0] = inData[0].tessOuterLeft;   // Left edge
    gl_TessLevelOuter[1] = inData[0].tessOuterBottom; // Bottom edge
    gl_TessLevelOuter[2] = inData[0].tessOuterRight;  // Right edge
    gl_TessLevelOuter[3] = inData[0].tessOuterTop;    // Top edge

    gl_TessLevelInner[0] = inData[0].tessInner;
    gl_TessLevelInner[1] = inData[0].tessInner;

    outData[gl_InvocationID].colour = inData[gl_InvocationID].colour;
    outData[gl_InvocationID].texCoord = inData[gl_InvocationID].texCoord;
    outData[gl_InvocationID].worldPos = inData[gl_InvocationID].worldPos;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
