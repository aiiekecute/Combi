#version 330 core

#define PI 3.1415926538

uniform vec3 point_pos;
uniform vec2 view_center;
uniform vec2 index_texcoord_center;

in vec2 position;
in vec2 texcoord;

out vec2 texcoord_out;
out vec2 numbers_texcoord_out;

void main(void)
{
	vec2 pos=point_pos.xy+vec2(position.x*cos(point_pos.z)+position.y*cos(point_pos.z+PI*0.5), position.x*sin(point_pos.z)+position.y*sin(point_pos.z+PI*0.5));

	gl_Position=vec4(5.0*(pos-view_center)/512.0, 0.0, 1.0);

	texcoord_out=texcoord/512.0;
	numbers_texcoord_out=index_texcoord_center+(pos-point_pos.xy)/120.0;
}
