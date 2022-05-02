#include "stdafx.h"
#include "windows.h"

#include "shader.h"

GLint ShaderStatus(GLuint shader, GLenum param)
{
    GLint status, length;
    GLchar buffer[1024];
    
    glGetShaderiv(shader, param, &status);
   
    if(status!= GL_TRUE) 
    {
		glGetShaderInfoLog(shader, 1024, &length, buffer);
		printf("Shader: %s\n", (const char*)buffer);
    }
     
    return status; 
}

bool CompileShader(GLuint *shader, const char *filename, GLenum type)//GL_VERTEX_SHADER,GL_FRAGMENT_SHADER;
{
	char temp_str[200];
	strcpy_s(temp_str,"shaders\\");
	strcat_s(temp_str,filename);

	HANDLE open_file=CreateFileA(temp_str, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(open_file==INVALID_HANDLE_VALUE) 
	{
		printf("shader compile error, can't read file: %s\n", filename);
		return false;
	}

	int source_size=GetFileSize(open_file, NULL);
	char *source=new char[source_size];
	int readed_bytes;
	ReadFile(open_file, source, source_size, (DWORD*) &readed_bytes, NULL);
   	CloseHandle(open_file);

	*shader=glCreateShader(type);
	glShaderSource(*shader, 1, (const GLchar**) &source, &source_size);
	glCompileShader(*shader);

	delete [] source; source=NULL;

	printf("SHADER NAME: %s\n", filename);
	if (ShaderStatus(*shader, GL_COMPILE_STATUS) != GL_TRUE) return false;
 
	GLint compileStatus;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus==0) 
	{
		printf("shader compile error, %s\n",filename);
		return false;
	}

	return true;
}

bool SHADER::CreateProgram(GLuint vs_shader, GLuint fs_shader)
{
	program=glCreateProgram();

	glAttachShader(program, vs_shader);
    glAttachShader(program, fs_shader);
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(!linked)
    {
        printf("Error create program\n");
		return false;
    }
	
	printf("shader program %d %d created.\n", vs_shader, fs_shader);
    return true;
}

int SHADER::GetAttribIndex(const char *name)
{
	int	loc=glGetAttribLocation(program, name);
	if(loc<0) 
	{
		printf ( "Error, GetAttribIndex\n" );
		return -1;
	}
    return loc;
}

bool SHADER::SetUniform(const char *name, float value[], int value_type)
{
	GLint loc=glGetUniformLocation(program, name);
	if(loc<0) 
	{
		printf("%s %d\n",name, strlen(name));
		printf("Error, SetUniform\n");
		return false;
	}

	switch(value_type)
	{
		case UFLOAT1:
			glUniform1f(loc, *value);
			break;
		case UFLOAT2:
			glUniform2f(loc, value[0], value[1]);
			break;
		case UFLOAT3:
			glUniform3f(loc, value[0], value[1], value[2]);
			break;
		case UFLOAT4:
			glUniform4f(loc, value[0], value[1], value[2], value[3]);
			break;
		case UFLOAT5:
			glUniform1fv(loc, 5, value);
		case UFLOAT6:
			glUniform1fv(loc, 6, value);
		case UMATRIX3:
			glUniformMatrix3fv(loc, 1, GL_TRUE, value);
			break;
		case UMATRIX4:
			glUniformMatrix4fv(loc, 1, GL_TRUE, value);
			break;
		case UINT1:
			glUniform1i(loc, int(*value));
			break;
	}
	
    return true;
}
