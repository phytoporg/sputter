#version 450 core
layout (location = 0) in ivec2 position;
layout (location = 1) in vec3 color;

out vec3 out_pixelColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    // Depth = 20.0, uniform this up later
    gl_Position = projection * view * vec4(position, 20.0, 1.0);
    out_pixelColor = color;
}
