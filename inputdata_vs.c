#version 330 core

in vec2 position;
in vec2 texcoord;

out vec2 texcoord_out;

void main(void)
{
	gl_Position=vec4(position*2.0-1.0, 0.0, 1.0);

	texcoord_out=texcoord;
}