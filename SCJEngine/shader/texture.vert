#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord0;

out vec2 TexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    TexUV=aTexCoord0;
    gl_Position=/**/projection*view*model*vec4(aPos,1);
}