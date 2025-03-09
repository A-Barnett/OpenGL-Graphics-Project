#version 430 core
in vec3 position;
in vec4 colour;
uniform mat4 modelMatrix; 
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out Vertex {
 vec4 colour;
} OUT;

void main() {
    //gl_Position =vec4(position, 1.0);
     gl_Position = projMatrix * viewMatrix *modelMatrix* vec4(position,1.0);
    OUT.colour = colour;
}
