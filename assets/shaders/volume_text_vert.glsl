#version 330 core
uniform vec2 rootPos;
uniform mat4 view;
uniform mat4 projection;
uniform float size;

layout(location=0) in vec3 position;
layout(location=3) in vec2 offset;

out vec3 fColor;

void main()
{
    gl_Position = projection * view * (vec4(position.xyz * size, 0.0) + vec4(rootPos + offset * size, 0.0, 1.0));
}
