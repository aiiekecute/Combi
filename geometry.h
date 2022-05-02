#pragma once
#include <GL/glew.h>

void SetVertexAttrib(int loc, int numComponents, GLsizei stride, GLsizei ptr);

class MESH_DATA
{
public:
	GLuint index;
	GLuint vbo[5];
	GLuint vao;

	float *vertices_data;
	int num_vertices;
	int vertex_size;
	int num_indexes;
	
	void CreateVAOandVBO(int num_vbo);
	void CreateVerticesData(int num_vert, int vert_size, float *v_data, int flag);
	void CreateIndexData(int num_index, int *v_data, int flag);
};