#version 460 core

uniform int process;
uniform sampler2D light;

in vec2 TexCoord;
out vec4 FragColor;

void main(){
    vec3 L=texture(light,TexCoord).xyz/process;
    FragColor=vec4(L/(L+vec3(0.5)),1);
}