#version 460 core

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 displacement;

void main()
{
   gl_Position = view * displacement * model * vec4(pos, 1.0);
}