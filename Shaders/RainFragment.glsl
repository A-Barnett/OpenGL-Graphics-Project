#version 330 core
out vec4 fragColour;

in Vertex {
    vec2 texCoord;
}IN;

void main() {
    if(IN.texCoord.x > 0.8 ||IN.texCoord.x < 0.2 ){
    discard;
    }else{
      fragColour = vec4(0.2,0.2,0.22,0.9);
    }
}
