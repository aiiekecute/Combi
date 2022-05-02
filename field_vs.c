#version 330 core

uniform vec2 view_center;

in vec2 position;
in vec2 texcoord;

out vec2 texcoord_out;
out vec2 texcoord_chess_out;

void main(void)
{
	gl_Position=vec4(5.0*(position-view_center)/512.0, 0.0, 1.0);

	texcoord_out=texcoord;
	texcoord_chess_out=texcoord*512.0;
}