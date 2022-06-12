#version 460 core

out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 camera_transform;
uniform mat4 projection;
uniform int rand_seed;

uniform samplerBuffer triangles;
uniform samplerBuffer BVH_nodes;
uniform sampler2D hdr;

in vec4 world_pos;

struct BVHNode{
    int left,right;
    int n,index,covered;
    vec3 AA,BB;
};

BVHNode load_bvh(int index){
    BVHNode res;
    int offset=index*4;
    ivec3 lr=ivec3(texelFetch(BVH_nodes,offset).xyz);
    ivec3 ni=ivec3(texelFetch(BVH_nodes,offset+1).xyz);
    res.left=lr.x;
    res.right=lr.y;
    res.index=ni.x;
    res.n=ni.y;
    res.AA=texelFetch(BVH_nodes,offset+2).xyz;
    res.BB=texelFetch(BVH_nodes,offset+3).xyz;
    return res;
}


float rand_seq=0;
float random (vec2 uv)
{
    return fract(sin(rand_seq+=0.054312+rand_seed*0.1433212+dot(uv.xy/vec2(1600,1600), vec2(12.9898,78.233))) * 43758.5453123);
}

float random ()
{
    return random(gl_FragCoord.xy);
}

vec3 random_in_unit_sphere()
{
    vec3 p;
    int i=0;
    do{
        p = 2.0*vec3(random(),random(),random()) - vec3(1,1,1);
        i++;
    }while (dot(p,p) >= 1.0 && i<10);
    return p;
}

vec3 random_in_half_sphere(in vec3 n){    
    vec3 r=random_in_unit_sphere();
    if(r==vec3(0))
        return n;
    vec3 res=normalize(r);

    return dot(res,n)>0?res:reflect(res,n);
}

bool hit_AABB(in vec3 pos,in vec3 dir,in vec3 AA, in vec3 BB){
    vec3 DAA=AA-pos;
    vec3 DBB=BB-pos;
    vec3 tA=DAA/dir;
    vec3 tB=DBB/dir;
    vec3 tMin=min(tA,tB);
    vec3 tMax=max(tA,tB);
    float tA_max=max(tMin.x,max(tMin.y,tMin.z));
    float tB_min=min(tMax.x,min(tMax.y,tMax.z));
    return tA_max<=tB_min?(tB_min>=0):false;
};

bool ray_hit_triangle(in vec3 ray_pos,in vec3 ray_dir, in vec3 v0,in vec3 v1,in vec3 v2,out float t,out vec3 hit_pt,out vec3 hit_normal)
{
    vec3 x=v1-v0;
    vec3 y=v2-v0;
    vec3 n=normalize(cross(x,y));
    float der=dot(ray_dir,n);
    if(der==0)
        return false;
    t=dot((v0-ray_pos),n)/der;
    if(t<0.001)
        return false;
    hit_pt=ray_pos+t*ray_dir;
    vec3 w=hit_pt-v0;
    float yy=dot(y,y);
    float xx=dot(x,x);
    float xy=dot(x,y);
    float xw=dot(x,w);
    float yw=dot(y,w);
    float ra=yy*xw-xy*yw;
    if(ra<0)
        return false;
    float sa=xx*yw-xy*xw;
    if(sa<0)
        return false;
    float det=xx*yy-xy*xy;
    if(ra+sa>det)
        return false;
    hit_normal=der>0?-n:n;
    return true;
}

bool ray_hit_triangle_with_normals(in vec3 ray_pos,in vec3 ray_dir, in mat3 v,in mat3 nn,out float t,out vec3 hit_pt,out vec3 hit_normal)
{
    vec3 x=v[1]-v[0];
    vec3 y=v[2]-v[0];
    vec3 n=normalize(cross(x,y));
    float der=dot(ray_dir,n);
    if(der==0)
        return false;
    t=dot((v[0]-ray_pos),n)/der;
    if(t<0.001)
        return false;
    hit_pt=ray_pos+t*ray_dir;
    vec3 w=hit_pt-v[0];
    float yy=dot(y,y);
    float xx=dot(x,x);
    float xy=dot(x,y);
    float xw=dot(x,w);
    float yw=dot(y,w);
    float ra=yy*xw-xy*yw;
    if(ra<0)
        return false;
    float sa=xx*yw-xy*xw;
    if(sa<0)
        return false;
    float det=xx*yy-xy*xy;
    if(ra+sa>det)
        return false;
    //hit_normal=der>0?-n:n;
    hit_normal=normalize((det-ra-sa)*nn[0]+ra*nn[1]+sa*nn[2]);
    return true;
}



bool hit_bvh(in vec3 ray_pos,in vec3 ray_dir,out float t,out vec3 hit_pt,out vec3 hit_normal,out int index_obj){
    ivec2 stack[16];//ivec2(process,parent_ptr)
    int stack_ptr=0;
    stack[0]=ivec2(0,-1);
    bool res=false;
    int is_leaf;
    int index_ptr=0;
    BVHNode current,child;
    current=load_bvh(0);

    t=1.0/+0.0;
    float current_t;
    vec3 current_pt;
    vec3 current_n;

    if(hit_AABB(ray_pos,ray_dir,current.AA,current.BB)){
        do{
            int proc_flag=stack[stack_ptr].x;
            is_leaf=2;
            switch(proc_flag){
            case 0:
                if(current.left!=-1){
                    child=load_bvh(current.left);
                    if(hit_AABB(ray_pos,ray_dir,child.AA,child.BB)){
                        stack[stack_ptr].x=1;
                        stack[++stack_ptr].y=index_ptr;
                        stack[stack_ptr].x=0;
                        index_ptr=current.left;
                        current=load_bvh(index_ptr);
                        break;
                    }
                }
                else{
                    is_leaf--;
                }
            case 1:
                if(current.right!=-1){
                    child=load_bvh(current.right);
                    if(hit_AABB(ray_pos,ray_dir,child.AA,child.BB)){
                        stack[stack_ptr].x=2;
                        stack[++stack_ptr].y=index_ptr;
                        stack[stack_ptr].x=0;
                        index_ptr=current.right;
                        current=load_bvh(index_ptr);
                        break;
                    }
                }
                else{
                    is_leaf--;
                }
            case 2:
                if(is_leaf==0){
                    for(int i=0;i<current.n;i++){
                        int obj_index=current.index+i;
                        mat3 v,n;
                        v[0]=texelFetch(triangles,obj_index*6).xyz;
                        v[1]=texelFetch(triangles,obj_index*6+2).xyz;
                        v[2]=texelFetch(triangles,obj_index*6+4).xyz;
                        n[0]=texelFetch(triangles,obj_index*6+1).xyz;
                        n[1]=texelFetch(triangles,obj_index*6+3).xyz;
                        n[2]=texelFetch(triangles,obj_index*6+5).xyz;

                        if(ray_hit_triangle_with_normals(ray_pos,ray_dir,v,n,current_t,current_pt,current_n)){
                            if(current_t<t){
                                t=current_t;
                                hit_pt=current_pt;
                                hit_normal=current_n;
                                index_obj=obj_index<2?0:2;
                            }
                            res=true;
                        }
                    }
                }
                if(stack_ptr!=0)
                    current=load_bvh(stack[stack_ptr].y);
                stack_ptr--;
            }
        }while(stack_ptr>=0);
    }
    return res;
}

void BRDF(in int id,in vec3 ray,in vec3 n,in float t,out vec3 ray_out,out vec3 light_e,out vec3 light_o)
{
    float energy;
    switch(id){
    case 0:
        ray_out=normalize(n + random_in_unit_sphere());
        light_e=vec3(0,0,0);
        light_o=vec3(0.8);
        break;
    case 1:
        ray_out=normalize(n + random_in_unit_sphere());
        light_e=10*vec3(1,1,1);
        light_o=vec3(1,1,1);
        break;
    case 2:
        energy=random();
        if(energy<0.2)
            ray_out=normalize(n +random_in_unit_sphere());
        else
            ray_out=normalize(reflect(ray,n)+0.2*random_in_unit_sphere());
        light_e=vec3(0,0,0);
        light_o=vec3(1,0.2,0.2);
        break;
    case 3:
        ray_out=normalize(n + random_in_unit_sphere());
        light_e=vec3(0,0,0);
        light_o=vec3(0.9,0.5,0.5);
        break;
    case 4:
        ray_out=normalize(n + random_in_unit_sphere());
        light_e=vec3(0,0,0);
        light_o=vec3(0.5,0.5,0.9);
        break;
    case 5:
        energy=random();
        if(energy<0.9){        
            if(dot(n,ray)<0)
                ray_out=refract(ray,n,0.666667);
            else {
                ray_out=refract(ray,-n,1.5);
            }
        }
        else{
            ray_out=reflect(ray,n);
        }
        if(ray_out==vec3(0))
            ray_out=reflect(ray,n);
        else
            ray_out=normalize(ray_out);
        light_e=vec3(0,0,0);
        light_o=vec3(1,1,1)*exp(-vec3(0.2,0.2,0)*t);
        break;
    default:
        ray_out=normalize(n +random_in_unit_sphere());
        light_e=vec3(0,0,0);
        light_o=vec3(1,1,1)*dot(n,ray_out)*2;
        break;
    }
}

vec3 rand_offset=vec3(0);

bool hit(in vec3 ray_pos,in vec3 ray_dir,out float t,out vec3 hit_pt,out vec3 hit_normal,out int index_obj)
{
    return hit_bvh(ray_pos,ray_dir,t,hit_pt,hit_normal,index_obj);
}

vec4 raytracing(in vec3 ray_pos,in vec3 ray_dir)
{
    int i=0;
    bool res=true;
    vec3 result_color=vec3(0);
    vec3 Lo=vec3(1);

    float t;
    vec3 hit_pt,hit_normal;
    int index_obj=0;

    for(int depth=0;depth<8;depth++)
    {
        vec3 dir=ray_dir;
        res=hit(ray_pos,ray_dir,t,hit_pt,hit_normal,index_obj);
        vec3 cur_e;
        vec3 cur_o;
        if(res)
        {
            BRDF(index_obj,dir,hit_normal,t,ray_dir,cur_e,cur_o);   
            ray_pos=hit_pt+0.001*ray_dir;       
            result_color+=cur_e*Lo;
            Lo*=cur_o;
        }else{
            float theta=atan(ray_dir.z,ray_dir.x)*0.15915494+0.5;
            float phi=atan(-ray_dir.y/length(ray_dir.xz))*0.15915494*2+0.5;
            vec4 h=texture(hdr,vec2(theta,phi));
            result_color+=h.xyz*Lo;
            //result_color=vec3((phi>0.2&&phi<0.4)&&(theta<0.1)?4:0.01)*Lo;
            break;
        }
    }

    return vec4(result_color,0);
}



void main(){
    vec2 screen=vec2(800,600);
    vec3 ray_pos=camera_transform[3].xyz;
    vec3 dir=world_pos.xyz-ray_pos;
    vec3 ray_dir=normalize(dir+dot(dir,camera_transform[2].xyz)*vec3(2*random()-1,2*random()-1,0)/vec3(screen*2,1));

    // float t;
    // vec3 hit_pt;
    // vec3 hit_normal;
    // bool res=hit_bvh(ray_pos,ray_dir,t,hit_pt,hit_normal);
    // FragColor=vec4(0,0,0,1);
    // if(res){
    //     FragColor.x=t*0.05;
    // } 

    int N=1;
    //if(rand_seed==-1)
    //    N=1;
    vec4 color=vec4(0,0,0,1);
    for(int i=0;i<N;i++){
        rand_offset=vec3(random()*0.1);
        color.xyz+=raytracing(ray_pos+rand_offset,ray_dir).xyz;
    }
    color.xyz/=N;
    FragColor=color;
    // if(ray_hit_triangle(ray_pos,ray_dir,v0,v1,v2,t,hit_pt,hit_normal)){
    //     FragColor=vec4(1,1,1,1);
    // }else{
    //     FragColor=vec4(0,0,0,1);
    // }

    //vec3 te=texelFetch(triangles,0).xyz/3;
    //FragColor=vec4(te,1);
}