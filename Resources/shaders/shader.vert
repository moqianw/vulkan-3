#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in	vec3 normal;
layout(location = 0) out vec4 fragColor;

layout(set = 0,binding = 0)  uniform camera{
    mat4 view;
    mat4 proj;
};
layout(push_constant) uniform Mutex{
    mat4 model;
};

void main(){

    gl_Position = proj *  view *  model * vec4(position,1.0);
    fragColor = color;
}