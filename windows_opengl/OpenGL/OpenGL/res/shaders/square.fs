#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D text_texture;
uniform sampler2D openGL_logo;

void main()
{
	// linearly interpolate between both textures (80% text, 20% openGL logo)
	FragColor = mix(texture(text_texture, TexCoord), texture(openGL_logo, TexCoord), 0.2);
}