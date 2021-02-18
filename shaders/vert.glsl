#version 330
#extension GL_ARB_gpu_shader_fp64: enable

uniform float iTime;

layout (location = 0) in vec3 pos;
out vec4 position;
//in vec4 pos;
void main()
{
    gl_Position = vec4(pos.xy-1, 1, 1);
    position = gl_Position;
}
