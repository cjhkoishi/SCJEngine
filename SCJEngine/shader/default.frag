#version 460 core

out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    FragColor=vec4(0.2,0.2,0.2,1);
}