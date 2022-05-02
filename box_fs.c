#version 330 core

uniform sampler2D boxTexture;
uniform sampler2D alphaTexture;
uniform sampler2D numbersTexture;

uniform vec2 index_texcoord_center;

uniform float kk;

in vec2 texcoord_out;
in vec2 numbers_texcoord_out;

out vec4 color;

void main(void)
{
	vec3 col=texture(boxTexture, texcoord_out).xyz;
	float alpha=texture(alphaTexture, texcoord_out).x;
	
	vec2 transform_numbers_texcoord_out;
	transform_numbers_texcoord_out.x=clamp(numbers_texcoord_out.x, index_texcoord_center.x-0.5/10.0, index_texcoord_center.x+0.5/10.0);
	transform_numbers_texcoord_out.y=clamp(numbers_texcoord_out.y, index_texcoord_center.y-0.5/30.0, index_texcoord_center.y+0.5/30.0);
	float numbers_alpha=texture(numbersTexture, transform_numbers_texcoord_out).x;

	color=vec4( mix(col.xyz*(1.0-alpha)+kk, vec3(0.0,0.0,1.0), numbers_alpha), 1.0 );
}