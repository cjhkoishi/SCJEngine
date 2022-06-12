#version 460 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;

void main(){
    gl_Position=vec4(aPos,1);
    TexCoord=aPos.xy/2+0.5;
}