#version 330 core
in vec3 norm;
in vec3 fragpos;
in vec2 uv;

out vec4 FragColor;

void main() {
    // Always white atm
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
