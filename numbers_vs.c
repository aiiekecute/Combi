#version 330 core

uniform vec2 point_pos;
uniform vec2 index_texcoord_center;

in vec2 position;
in vec2 texcoord;

out vec2 texcoord_out;

void main(void)
{
	gl_Position=vec4((point_pos+position*vec2(0.012,0.008))*2.0-1.0, 0.0, 1.0);

	texcoord_out=index_texcoord_center+texcoord*vec2(0.03,0.01);
}