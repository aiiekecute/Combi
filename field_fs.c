#version 330 core

uniform sampler2D Texture;

in vec2 texcoord_out;
in vec2 texcoord_chess_out;

out vec4 color;

void main(void)
{
	vec3 col=texture(Texture, texcoord_out).xyz;
	float koef=(int(texcoord_chess_out.x)%2+int(texcoord_chess_out.y)%2)%2;
	color=vec4( col+vec3(0.0,1.0,0.0)*koef*0.3, 1.0 );
}