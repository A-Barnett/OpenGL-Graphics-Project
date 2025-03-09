#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 startingPos;


in vec3 position;
in vec2 texCoord;
in vec3 normal;   
in vec3 tangent;   
in vec3 worldPos;

out Vertex {
    vec2 texCoord;
    vec3 normal;  
    vec3 tangent;  
    vec3 worldPos;

} OUT;

void main(void) {
    // Transform position by modelMatrix and account for any object translation.
    vec4 transformedPos = modelMatrix * vec4(position + startingPos, 1.0);

    // Use the transformed position as world position
    OUT.worldPos = transformedPos.xyz;
    

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 wNormal = normalize(normalMatrix * normalize(normal));
    vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

    OUT.normal = wNormal;
    OUT.tangent = wTangent;

    // Pass the other attributes
   // OUT.normal = normalize(mat3(modelMatrix) * normal); // Apply model matrix rotation to normal
  //  OUT.tangent = normalize(mat3(modelMatrix) * tangent); // Apply model matrix rotation to tangent
    OUT.texCoord = texCoord;
    // Compute final clip space position
    gl_Position = projMatrix * viewMatrix * transformedPos;

}

//OpenGL Debug Output: Source(OpenGL), Type(Error), Priority(High), Error has been generated. GL error GL_INVALID_OPERATION in Uniform3fv: (ID: 4095292898)
//GL error GL_INVALID_OPERATION: 
//mismatched type setting uniform of location "10" in program 9, "" using shaders, 10, "IslandVertex.glsl", 11, "IslandFragment.glsl"