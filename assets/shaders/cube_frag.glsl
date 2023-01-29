#version 330 core
in vec3 norm;
in vec3 fragPos;
in vec2 uv;
uniform vec3 light;
uniform sampler2D tex0;
uniform vec3 color;
out vec4 FragColor;

void main() {
    FragColor = vec4(color, 1.0);
}
