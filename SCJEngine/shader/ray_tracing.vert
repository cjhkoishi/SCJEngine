#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 camera_transform;
uniform mat4 projection;

uniform samplerBuffer triangles;
uniform samplerBuffer BVH_nodes;

out vec4 world_pos;

void main(){
    world_pos=model*vec4(aPos,1);
    gl_Position=projection*view*world_pos;
}