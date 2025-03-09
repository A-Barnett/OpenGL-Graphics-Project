#version 430 core
layout(quads, equal_spacing, ccw) in;

in VertexData {
    vec4 colour;
    vec2 texCoord;
    vec3 worldPos;
} inData[];

out VertexData {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} outData;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time;
uniform float amplitude;
uniform float frequency;
uniform float waveSpeed;
uniform float amplitudeScale;
uniform float posChangeX;
uniform float posChangeZ;
uniform float textureScale;
uniform float directionBiasStrength;
uniform int waveCount;

float calculateWaveDisplacement(vec3 pos) {
    float waveSum = 0.0;
    for (int i = 2; i <= waveCount; i++) {

        float directionBias = mix(1.0, float(i) / waveCount, -directionBiasStrength);
        float directionX = directionBias + mod(float(i) * 0.3, 2.0);
        float directionZ = directionBias + mod(float(i) * 0.7, 3.0);

        waveSum += (amplitude * pow(amplitudeScale, float(i))) * 
                   sin((frequency * i) * (directionX * pos.x + directionZ * pos.z) + (time * float(i / 5) * waveSpeed));
    }
    return waveSum;
}

float calculateWaveDerivative(vec3 pos, vec3 direction) {
    float waveSum = 0.0;
    for (int i = 2; i <= waveCount; i++) {

        float directionBias = mix(1.0, float(i) / waveCount, -directionBiasStrength);
        float directionX = directionBias + mod(float(i) * 0.3, 2.0);
        float directionZ = directionBias + mod(float(i) * 0.7, 3.0);
        float phase = (frequency * i) * (directionX * pos.x + directionZ * pos.z) + (time * float(i / 5) * waveSpeed);

        waveSum += (amplitude * pow(amplitudeScale, float(i))) * 
                   cos(phase) * (frequency * i) * dot(direction, vec3(directionX, 0.0, directionZ));
    }
    return waveSum;
}


void main(void) {
    // Base position and displaced position
    vec3 p0 = inData[0].worldPos;
    vec3 p1 = inData[1].worldPos;
    vec3 p2 = inData[2].worldPos;
    vec3 p3 = inData[3].worldPos;

    vec3 pos = mix(mix(p0, p1, gl_TessCoord.x), mix(p3, p2, gl_TessCoord.x), gl_TessCoord.y);
    pos.x += posChangeX;
    pos.z += posChangeZ;

    float waveSum = calculateWaveDisplacement(pos);
    vec3 displacedPos = pos + vec3(0.0, waveSum, 0.0);

    // Calculate tangent and binormal using pixel-perfect derivatives
    vec3 dPosX = vec3(1.0, calculateWaveDerivative(pos, vec3(1.0, 0.0, 0.0)), 0.0);  // Tangent in X direction
    vec3 dPosZ = vec3(0.0, calculateWaveDerivative(pos, vec3(0.0, 0.0, 1.0)), 1.0);  // Binormal in Z direction

    // Calculate normal as cross product of tangents, then normalize
    vec3 normal = normalize(cross(dPosZ, dPosX));
    vec3 tangent = normalize(dPosX);
    vec3 binormal = cross(normal, tangent); // Ensure binormal is perpendicular

    // Pass data to fragment shader
    outData.worldPos = displacedPos;
    outData.colour = mix(mix(inData[0].colour, inData[1].colour, gl_TessCoord.x), mix(inData[3].colour, inData[2].colour, gl_TessCoord.x), gl_TessCoord.y);

    // Calculate texture coordinates based on the global world position
    outData.texCoord = (displacedPos.xz) * textureScale;

    outData.normal = normal;
    outData.tangent = tangent;
    outData.binormal = binormal;

    gl_Position = projMatrix * viewMatrix * vec4(outData.worldPos, 1.0);
}
