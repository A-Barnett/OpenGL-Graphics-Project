#version 330 core

uniform sampler2D sceneTex;
uniform sampler2D depthTex;
uniform float fogStart;
uniform float fogEnd;
uniform bool fogEnabled;
uniform vec3 cameraPos;
uniform vec3 cameraDirection; // Updated to vec3 for the camera's direction
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float lightAngle;
uniform vec3 lightDirection;
float spotlightIntensity = 15;

float nearPlane = 1.0;
float farPlane = 100000.0;

in Vertex {
    vec2 texCoord;
} IN;

out vec4 fragColor;

void main() {
    
        vec4 colour = texture(sceneTex, IN.texCoord);

        if(fogEnabled){

        
        float depthVal = texture(depthTex, IN.texCoord).r;

        // Calculate real depth in world units
        float realDepth = nearPlane + (depthVal * (farPlane - nearPlane));

        // Calculate the clip space position
        vec4 clipSpacePos = vec4((IN.texCoord * 2.0) - 1.0, depthVal * 2.0 - 1.0, 1.0);

        // Transform clip space position to view space
        vec4 viewSpacePos = inverse(projMatrix) * clipSpacePos;
        viewSpacePos /= viewSpacePos.w; // Perspective divide

        // Transform view space position to world space
        vec3 fragPos = (inverse(viewMatrix) * viewSpacePos).xyz;

        // Check if the fragment is very close to the light position
        float distanceToLight = length(fragPos - lightPos);
        if (distanceToLight < 10000.0) {
            fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Set to red
            return;
        }

        // Calculate spotlight effect on fog
        vec3 lightToFrag = normalize(fragPos - lightPos);
        float dist = length(fragPos - lightPos);
        float spotEffect = max(dot(normalize(lightDirection), lightToFrag), 0.0);
        float attenuation = max(1.0 - (dist / (lightRadius * 2)), 0.0);
        float spotlightFactor = pow(spotEffect, lightAngle) * attenuation * spotlightIntensity;

        // Check if the depth is within the fog range
        if (depthVal >= fogStart) {
            float fogFactor = clamp((depthVal - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
            float fogAlpha = pow(fogFactor, 2.0);

            vec3 fogColor = mix(vec3(0.3), lightColour.rgb, (spotlightFactor / 4));
            colour.rgb = mix(colour.rgb, fogColor, fogAlpha);
            colour.a = fogAlpha * (1.0 - (spotlightFactor * 1.5));
        }

        fragColor = colour;

        // Calculate the direction from the camera to the light
        vec3 camToLightDir = normalize(cameraPos - lightPos);

        // Calculate the angle between the camera's direction and the direction to the light
        float angleBetween = acos(dot(normalize(cameraDirection), camToLightDir)); // Angle in radians

        // Check if the angle is within the light's spotlight angle
        float lightConeAngle = radians(lightAngle); // Convert lightAngle to radians if it's in degrees

        // Calculate alignment of the camera's direction with the light direction
        float alignment = max(dot(normalize(cameraDirection), normalize(lightDirection)), 0.0);

        // Calculate distance factor
        float distFactor = clamp((1.0 - (dist / lightRadius))/4, 0.0, 1.0);

        // Amplify bloom effect based on alignment and distance to light
        float bloomFactor = pow(alignment, 30.0) * spotlightIntensity * (distFactor) * (max(0, 1 - angleBetween) * 4) * 0.5;
        vec4 bloomColor = vec4(lightColour.rgb * bloomFactor * 0.5, 1);

        fragColor += bloomColor;
        }else{
        if(colour.r >= 0.85 && colour.g >= 0.85 && colour.b >= 0.85){
            colour.rgb += 0.08;
        }
        fragColor = colour;
      }
    
}
