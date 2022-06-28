#version 460 core

out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

void main(){
    FragColor=color;
}