#version 330 core

layout (location = 0) in vec2 vertex_position;
layout (location = 1) in vec2 vertex_texcoords;

out vec2 texcoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    // Z é sempre 0 e W é 1
    gl_Position = projection * model * vec4(vertex_position, 0.0, 1.0);
    texcoords = vertex_texcoords;
}