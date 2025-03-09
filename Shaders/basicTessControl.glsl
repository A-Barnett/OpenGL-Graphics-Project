#version 430 core
layout(vertices = 4) out;
uniform float tessLevel;
uniform float tessInner;
uniform float tessOuter;


void main() {
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = tessLevel+tessInner;
        gl_TessLevelInner[1] = tessLevel+tessInner;
        gl_TessLevelOuter[0] = tessLevel+tessOuter;
        gl_TessLevelOuter[1] = tessLevel+tessOuter;
        gl_TessLevelOuter[2] = tessLevel+tessOuter;
        gl_TessLevelOuter[3] = tessLevel+tessOuter;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  
}
