#pragma once
#include <GL/glew.h>

enum UNIFORM_TYPE {UFLOAT1, UFLOAT2, UFLOAT3, UFLOAT4, UFLOAT5, UFLOAT6, UMATRIX3, UMATRIX4, UINT1};

bool OpenShaderArchive(const char *filename);
bool CompileShader(GLuint *shader, const char *filename, GLenum type);

class SHADER
{
public:
	GLuint program;
	bool CreateProgram(GLuint vs_shader, GLuint fs_shader);
	int GetAttribIndex(const char *name);
	bool SetUniform(const char *name, float value[], int value_type);
};