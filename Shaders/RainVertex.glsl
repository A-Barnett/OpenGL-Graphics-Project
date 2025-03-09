#version 330 core

in vec3 position;
in vec2 texCoord;
layout(location = 2) in vec3 instancePosition;

uniform mat4 modelMatrix; 
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out Vertex {
    vec2 texCoord;
} OUT;

void main() {
    // Extract the camera right vector from the view matrix
    vec3 cameraRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    // Use a fixed up vector (0,1,0) to maintain the vertical orientation
    vec3 cameraUp = vec3(0.0, 1.0, 0.0);

    // Apply a 10-degree rotation around the z-axis to simulate wind
    float tiltAngle = radians(7.0);
    mat3 tiltMatrix = mat3(
        cos(tiltAngle), -sin(tiltAngle), 0.0,
        sin(tiltAngle),  cos(tiltAngle), 0.0,
        0.0,            0.0,            1.0
    );

    // Calculate the quad's world position with the tilt
    vec3 worldPos = instancePosition + tiltMatrix * (position.x * cameraRight + position.y * cameraUp);

    // Transform the world position to clip space
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(worldPos, 1.0);

    OUT.texCoord = vec2(position.x, position.z) + 0.5f;
}
