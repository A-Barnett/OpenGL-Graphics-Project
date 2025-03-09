#version 430 core
layout(quads, equal_spacing, ccw) in;

patch in VertexData {
    vec4 colour;
} inData;

out VertexData {
    vec4 colour;
} outData;


uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform float time; // Time variable to animate the ripple effect
uniform float scale;

// Define the control points for the Bézier surface
vec3 bezierControlPoints[3] = vec3[3](
    vec3(-1.0, 0.0, 1.0),  // Control point 1
    vec3(0.0, 1.0, 0.0),    // Control point 2 (raised higher)
    vec3(1.0, 0.0, -1.0)    // Control point 3
);

float bezierCurve(float t) {
    // Quadratic Bézier blending function
    return (1.0 - t) * (1.0 - t) * bezierControlPoints[0].y +
           2.0 * (1.0 - t) * t * bezierControlPoints[1].y +
           t * t * bezierControlPoints[2].y;
}

void main() {
    // Obtain the positions of each vertex in the quad
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    vec3 pos = mix(mix(p0, p1, gl_TessCoord.x), mix(p3, p2, gl_TessCoord.x), gl_TessCoord.y);

    float bezierHeight = bezierCurve(gl_TessCoord.x);

    float waveHeight = 0.2 * sin(gl_TessCoord.x * 5.0 + time * 2.0) * sin(gl_TessCoord.y * 5.0 + time * 2.0);

    pos.y += (bezierHeight + waveHeight)*scale;

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(pos, 1.0);
       float heightFactor = pos.y; 
    outData.colour = vec4(1.0, 0.5, 0.2, 1.0);
                          
}
                    
