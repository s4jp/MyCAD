#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 displacement;
void main()
{
   gl_Position = proj * displacement * view *  model * vec4(pos, 1.0);
   TexCoord = uv;
}