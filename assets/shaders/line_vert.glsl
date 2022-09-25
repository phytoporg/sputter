#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 out_pixelColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
    // Depth = 0, parameterize this later
    gl_Position = projection * view * vec4(position, 1.0);
    out_pixelColor = color;
}
