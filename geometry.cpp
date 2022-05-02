#include "stdafx.h"

#include "geometry.h"

void SetVertexAttrib(int loc, int numComponents, GLsizei stride, GLsizei ptr)
{
	GLenum type=GL_FLOAT;
	GLboolean normalized=GL_FALSE;

	stride*=sizeof(float);
	ptr*=sizeof(float);
	   
	glVertexAttribPointer(loc, numComponents, type, normalized, stride, (const GLvoid*) ptr);          
    glEnableVertexAttribArray(loc);
}

void MESH_DATA::CreateVAOandVBO(int num_vbo)
{
	glGenVertexArrays(1,&vao);
	if(num_vbo>0) glGenBuffers(num_vbo, vbo);
	glGenBuffers(1,&index);
}

void MESH_DATA::CreateVerticesData(int num_vert, int vert_size, float *v_data, int flag)//flag=GL_STATIC_DRAW,GL_DYNAMIC_DRAW
{
	num_vertices=num_vert;
	vertex_size=vert_size*sizeof(float);
	vertices_data=v_data;

	glBufferData(GL_ARRAY_BUFFER, num_vertices*vertex_size, vertices_data, flag);
}

void MESH_DATA::CreateIndexData(int num_index, int *v_data, int flag)
{
	num_indexes=num_index;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indexes*sizeof(int), v_data, flag);//flag=GL_STATIC_DRAW,GL_DYNAMIC_DRAW		
}
