#version 330 core
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;
in ivec2 position;
out vec3 fragPos;
out vec3 fragColor;

void main() {
    // Depth = 20.0
    gl_Position = projection * view * vec4(position, 20.0, 1.0);
    fragPos = vec3(model * vec4(position, 1.0));
    fragColor = color;
}
