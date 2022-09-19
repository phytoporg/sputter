#version 450 core
in vec3 out_pixelColor;
out vec4 FragColor;

void main() 
{
    FragColor = vec4(out_pixelColor, 1.0);
}