#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float smoothness; 
uniform float metallic; 
uniform float normalMapScrollSpeed; // New uniform variable
uniform float time; // Use the existing time uniform

in Vertex {
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
    vec3 incident = normalize(lightPos - IN.worldPos);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfDir = normalize(incident + viewDir);

    mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

    // Scroll the normal map with additional time-based perturbation
    vec2 scrolledTexCoord = IN.texCoord + vec2(time * normalMapScrollSpeed, time * normalMapScrollSpeed);
    vec2 perturbation = vec2(sin(time * 0.5), cos(time * 0.5)) * 3; // Adjust the 0.05 factor for more or less perturbation
    vec2 finalTexCoord = scrolledTexCoord + perturbation;

    vec4 diffuse = texture(diffuseTex, finalTexCoord);
    vec3 bumpNormal = texture(bumpTex, finalTexCoord).rgb;
    bumpNormal = normalize(TBN * (bumpNormal * 2.0 - 1.0));

    float lambert = max(dot(incident, bumpNormal), 0.0);
    float distance = length(lightPos - IN.worldPos);
    float attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);

    float specFactor = pow(max(dot(halfDir, bumpNormal), 0.0), smoothness * 128.0) * metallic;

    vec3 surface = diffuse.rgb * lightColour.rgb;
    fragColour.rgb = surface * lambert * attenuation;
    fragColour.rgb += lightColour.rgb * specFactor * attenuation * 0.33;
    fragColour.rgb += surface * 0.3;
    fragColour.a = diffuse.a;
    fragColour *= IN.colour;
}
