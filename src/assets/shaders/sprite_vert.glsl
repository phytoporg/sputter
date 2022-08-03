#version 430
// <vec3 position, vec2 texCoords>
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 texCoords; 

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	TexCoords = texCoords;
	gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
