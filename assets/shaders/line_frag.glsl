#version 450 core
uniform vec3 borderColor;
out vec4 FragColor;

void main() 
{
    FragColor = vec4(borderColor, 1.0);
}