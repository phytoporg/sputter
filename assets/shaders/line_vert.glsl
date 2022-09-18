#version 450 core
uniform mat4 view;
uniform mat4 projection;
in ivec2 position;

void main() {
    // Depth = 20.0, uniform this up later
    gl_Position = projection * view * vec4(position, 20.0, 1.0);
}
