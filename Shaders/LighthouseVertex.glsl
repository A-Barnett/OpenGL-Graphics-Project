#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;
uniform vec3 startingPos;
uniform float textureScale; // Add textureScale uniform
uniform float textureRotation; // Add textureRotation uniform

in vec3 position;
in vec2 texCoord;
in vec3 normal;   
in vec3 tangent;   
in vec3 worldPos;

out Vertex {
      vec2 texCoord;
    vec4 colour;
    vec3 normal;   
    vec3 tangent;
    vec3 worldPos; 
} OUT;

void main(void) {
    vec3 worldPos = position;
    worldPos += startingPos;

    // Apply texture scaling
    vec2 scaledTexCoord = texCoord * textureScale;
    
    // Apply texture rotation
    float cosRotation = cos(textureRotation);
    float sinRotation = sin(textureRotation);
    mat2 rotationMatrix = mat2(
        cosRotation, -sinRotation,
        sinRotation, cosRotation
    );
    vec2 rotatedTexCoord = rotationMatrix * scaledTexCoord;

    OUT.texCoord = rotatedTexCoord; // Use rotated texture coordinates
    OUT.colour = nodeColour;


     vec4 transformedPos = modelMatrix * vec4(position + startingPos, 1.0);

    // Use the transformed position as world position
    OUT.worldPos = transformedPos.xyz;

    // Pass the other attributes
    OUT.normal = normalize(mat3(modelMatrix) * normal); // Apply model matrix rotation to normal
    OUT.tangent = normalize(mat3(modelMatrix) * tangent); // Apply model matrix rotation to tangent


    // Compute final clip space position
    gl_Position = projMatrix * viewMatrix * transformedPos;
}
