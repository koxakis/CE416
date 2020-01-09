#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D exhibit_5_texture_1;
uniform sampler2D exhibit_5_texture_2;

void main()
{
	// linearly interpolate between both textures (80% text, 20% openGL logo)
	FragColor = mix(texture(exhibit_5_texture_1, TexCoord), texture(exhibit_5_texture_2, TexCoord), 0.2);
}