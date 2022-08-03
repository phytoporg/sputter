#version 430
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;

void main()
{    
	color = texture(image, TexCoords);
	// color = vec4(1.0, 0.0, 0.0, 1.0);
	// Something more interesting while we debug textures
	//color = vec4(
	//    gl_FragCoord.x / 1024.0,
	//    gl_FragCoord.y / 768.0,
	//    0.0, 1.0);
}
