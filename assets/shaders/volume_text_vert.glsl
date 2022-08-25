#version 330 core
uniform vec2 rootPos;
uniform float size;
uniform vec2 offset;

out vec3 fColor;

void main()
{
    gl_Position = vec4(rootPos + offset, 0.0, 1.0);
}
