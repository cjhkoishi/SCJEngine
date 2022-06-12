#version 460 core

in vec4 world_pos;
out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 camera_transform;
uniform mat4 projection;

vec3 ambient_color=vec3(1,1,1);
vec3 specular_color=vec3(1,1,1);
vec3 diffuse_color=vec3(1,1,1);

vec3 blinn(in vec3 light,in vec3 eye,in vec3 normal)
{
    light=normalize(light);
    eye=normalize(eye);

    vec3 halfway=-normalize(light+eye);

    float costh=dot(normal,-light);
    float diffuse=max(0,costh);
    float specular=0;
    if(costh>0)
        specular=max(0,pow(dot(halfway,normal),16));
    vec3 color=ambient_color*0.05 + diffuse_color*0.4*diffuse + specular_color*0.3*specular;

    return color;
}

void main(){
    vec3 normal=normalize(cross(dFdx(world_pos).xyz,dFdy(world_pos).xyz));
    vec3 light=(camera_transform*vec4(-2,-2,-1,0)).xyz;
    vec3 eye=world_pos.xyz-camera_transform[3].xyz;
    FragColor = vec4(blinn(light,eye,normal),1);
}