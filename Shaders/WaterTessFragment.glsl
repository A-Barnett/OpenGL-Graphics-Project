#version 430 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform samplerCube cubeTex;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float lightAngle;      
uniform vec3 lightDirection;
uniform float spotlightIntensity; 
uniform vec4 lightColour2;
uniform vec3 lightDirection2;
uniform float lightRadius2;    
uniform float smoothness;
uniform float metallic;
uniform float time;
uniform float reflections;

in VertexData {
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
    vec2 scaledTexCoord = IN.texCoord;


    vec4 diffuse = texture(diffuseTex, scaledTexCoord);
    vec3 bumpNormal = texture(bumpTex, scaledTexCoord).rgb;
    bumpNormal = normalize(TBN * (bumpNormal * 2.0 - 1.0));


    float lambert = max(dot(incident, bumpNormal), 0.0);
    float distance = length(lightPos - IN.worldPos);
    float attenuation = 1.0 / (1.0 + (distance / lightRadius) * (distance / lightRadius));

    float spotEffect = dot(incident, normalize(-lightDirection));
    float spotCutoff = cos(radians(lightAngle * 0.5)); 

    float spotlightConeEffect = clamp((spotEffect - spotCutoff) / (1.0 - spotCutoff), 0.0, 1.0);

    lambert *= spotlightConeEffect * spotlightIntensity;
    attenuation *= spotlightConeEffect * spotlightIntensity;

    float specFactor = pow(max(dot(halfDir, bumpNormal), 0.0), smoothness * 128.0) * metallic * spotlightIntensity;

 
    float directionalLambert = max(dot(normalize(-lightDirection2), bumpNormal), 0.0);
    float directionalIntensity = lightRadius2; 
    vec3 surface = diffuse.rgb * lightColour.rgb;
    vec3 finalColor = surface * lambert * attenuation;
    finalColor += lightColour.rgb * specFactor * attenuation * 0.33;
    finalColor += surface * 0.1; // Ambient

    finalColor += lightColour2.rgb * surface * directionalLambert * directionalIntensity;
    finalColor += lightColour2.rgb * surface * directionalLambert * directionalIntensity * specFactor;

    fragColour.rgb = finalColor;
    fragColour.a = diffuse.a;
    fragColour *= IN.colour;

 

    vec3 reflectDir = reflect(-viewDir, normalize(IN.normal));
    vec4 reflectTex = texture(cubeTex, reflectDir);
    fragColour += (reflectTex*reflections);
    

}
