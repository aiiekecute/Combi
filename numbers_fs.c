#version 330 core

uniform sampler2D Texture;

in vec2 texcoord_out;

out vec4 color;

void main(void)
{
	float col=texture(Texture, texcoord_out).x;
	color=vec4(vec3(0.0,1.0,0.0), col.x);
}