#version 330 core

uniform sampler2D sandTex;
uniform sampler2D gravelTex;
uniform sampler2D cliffTex;
uniform sampler2D grassTex;

uniform sampler2D sandNormal;
uniform sampler2D gravelNormal;
uniform sampler2D cliffNormal;
uniform sampler2D grassNormal;

uniform float maxSandHeight;
uniform float maxGravelHeight;
uniform float maxCliffHeight;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float lightAngle;      
uniform vec3 lightDirection;
uniform float spotlightIntensity=1.5; 
uniform vec3 lightPos2;
uniform vec4 lightColour2;
uniform vec3 lightDirection2;
uniform float lightRadius2;    
uniform float smoothness;
uniform float metallic;

float blendDistance = 50;

in Vertex {
    vec2 texCoord;
    vec3 normal;   
    vec3 tangent; 
    vec3 worldPos; 
} IN;

out vec4 fragColour;

void main(void) {
       float height = IN.worldPos.y;
    vec4 tex1, tex2;
    vec3 normal1, normal2;
    float blendFactor = 0.0;

    // Determine blend factor and choose textures/normals to blend between
    if (height <= maxSandHeight) {
        tex1 = texture(sandTex, IN.texCoord);
        normal1 = texture(sandNormal, IN.texCoord).rgb;

        if (height <= maxSandHeight - blendDistance) {
            tex2 = texture(sandTex, IN.texCoord);
            normal2 = texture(sandNormal, IN.texCoord).rgb;
            blendFactor = 0.0;
        } else if (height <= maxSandHeight) {
            tex2 = texture(gravelTex, IN.texCoord);
            normal2 = texture(gravelNormal, IN.texCoord).rgb;
            blendFactor = (height - (maxSandHeight - blendDistance)) / blendDistance;
        }
    } else if (height <= maxGravelHeight) {
        tex1 = texture(gravelTex, IN.texCoord);
        normal1 = texture(gravelNormal, IN.texCoord).rgb;

        if (height <= maxGravelHeight - blendDistance) {
            tex2 = texture(gravelTex, IN.texCoord);
            normal2 = texture(gravelNormal, IN.texCoord).rgb;
            blendFactor = 0.0;
        } else if (height <= maxGravelHeight) {
            tex2 = texture(cliffTex, IN.texCoord);
            normal2 = texture(cliffNormal, IN.texCoord).rgb;
            blendFactor = (height - (maxGravelHeight - blendDistance)) / blendDistance;
        }
    } else if (height <= maxCliffHeight) {
        tex1 = texture(cliffTex, IN.texCoord);
        normal1 = texture(cliffNormal, IN.texCoord).rgb;

        if (height <= maxCliffHeight - blendDistance) {
            tex2 = texture(cliffTex, IN.texCoord);
            normal2 = texture(cliffNormal, IN.texCoord).rgb;
            blendFactor = 0.0;
        } else if (height <= maxCliffHeight) {
            tex2 = texture(grassTex, IN.texCoord);
            normal2 = texture(grassNormal, IN.texCoord).rgb;
            blendFactor = (height - (maxCliffHeight - blendDistance)) / blendDistance;
        }
    } else {
        tex1 = texture(grassTex, IN.texCoord);
        normal1 = texture(grassNormal, IN.texCoord).rgb;
        tex2 = texture(grassTex, IN.texCoord);
        normal2 = texture(grassNormal, IN.texCoord).rgb;
        blendFactor = 0.0;
    }

    // Interpolate between textures and normals
    vec4 selectedTexture = mix(tex1, tex2, blendFactor);
    vec3 sampledNormal = mix(normal1, normal2, blendFactor);

    // Transform sampled normal from [0,1] to [-1,1]
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);

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
    vec3 surfaceColor = selectedTexture.rgb;
    vec3 color1 = surfaceColor * lightColour.rgb * lambert1 * attenuation1 + specFactor1 * lightColour.rgb * attenuation1 * 0.33;
    vec3 color2 = surfaceColor * lightColour2.rgb * lambert2 * directionalIntensity + specFactor2 * lightColour2.rgb * directionalIntensity * 0.33;

    vec3 ambient = surfaceColor * 0.3; // Ambient lighting
    fragColour.rgb = color1 + color2 + ambient;
    fragColour.a = selectedTexture.a;

}
