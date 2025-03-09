#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform vec4 lightColour2;
uniform vec3 lightDirection2;
uniform float lightRadius2;    
uniform float smoothness;
uniform float metallic;
uniform vec3 cameraPos;
uniform bool transparent;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float lightAngle;      
uniform vec3 lightDirection;
uniform float spotlightIntensity=5;

in Vertex {
    vec2 texCoord;
    vec4 colour;
    vec3 normal;   
    vec3 tangent;
    vec3 worldPos; 
} IN;

out vec4 fragColour;

void main(void) {
    // Calculate diffuse color
    vec4 diffuse = texture(diffuseTex, IN.texCoord);

    // Calculate normal from normal map
    vec3 sampledNormal = texture(normalTex, IN.texCoord).rgb;
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0); // Transform from [0,1] to [-1,1]

    // Calculate binormal (bitangent) from normal and tangent
    vec3 binormal = normalize(cross(IN.normal, IN.tangent));
    
    // Convert sampled normal from tangent space to world space using TBN matrix
    mat3 TBN = mat3(normalize(IN.tangent), binormal, normalize(IN.normal));
    vec3 worldNormal = normalize(TBN * sampledNormal);

    vec3 viewDir = normalize(cameraPos - IN.worldPos);

    // Spotlight calculations (light1)
    vec3 incident1 = normalize(lightPos - IN.worldPos);
    float lambert1 = max(dot(incident1, worldNormal), 0.0);
    float distance1 = length(lightPos - IN.worldPos);
    float attenuation1 = 1.0 / (1.0 + (distance1 / lightRadius) * (distance1 / lightRadius));

    float spotEffect = dot(incident1, normalize(-lightDirection));
    float spotCutoff = cos(radians(lightAngle * 0.5)); 
    float spotlightConeEffect = clamp((spotEffect - spotCutoff) / (1.0 - spotCutoff), 0.0, 1.0);

    lambert1 *= spotlightConeEffect * spotlightIntensity;
    attenuation1 *= spotlightConeEffect * spotlightIntensity;

    vec3 halfDir1 = normalize(incident1 + viewDir);
    float specFactor1 = pow(max(dot(halfDir1, worldNormal), 0.0), smoothness * 128.0) * metallic * spotlightIntensity;

    // Directional light calculations (light2)
    vec3 incident2 = normalize(-lightDirection2);
    float lambert2 = max(dot(incident2, worldNormal), 0.0);
    float directionalIntensity = lightRadius2;

    vec3 halfDir2 = normalize(incident2 + viewDir);
    float specFactor2 = pow(max(dot(halfDir2, worldNormal), 0.0), smoothness * 128.0) * metallic;

    // Combine light effects with selected texture color
    vec3 surfaceColor = diffuse.rgb;
    vec3 color1 = surfaceColor * lightColour.rgb * lambert1 * attenuation1 + specFactor1 * lightColour.rgb * attenuation1 * 0.33;
    vec3 color2 = surfaceColor * lightColour2.rgb * lambert2 * directionalIntensity + specFactor2 * lightColour2.rgb * directionalIntensity * 300;

    vec3 ambient = surfaceColor * 0.1; // Ambient lighting
    fragColour.rgb = color1 + color2 + ambient;
    fragColour.a = diffuse.a;
    fragColour *= IN.colour;

    // Adjust transparency if the transparent uniform is true
    if (transparent) {
        fragColour.a *= 0.5; // 90% transparent (10% opacity)
    }
}
