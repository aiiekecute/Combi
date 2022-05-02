#version 330 core

uniform sampler2D Texture;

in vec2 texcoord_out;

out vec4 color;

void main(void)
{
	vec3 col=texture(Texture, texcoord_out).xyz;
	color=vec4(vec3(0.0,0.0,1.0)*(1.0-col.x), 1.0);
}