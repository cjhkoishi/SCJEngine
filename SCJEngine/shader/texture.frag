#version 460 core

in vec2 TexUV;
out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D texture0;

void main(){
    FragColor=texture(texture0, TexUV);
}