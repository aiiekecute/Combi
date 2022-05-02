#pragma once

#include <GL/glew.h>
#include "windows.h"

struct IMAGE_INFO
{
	int size_x, size_y;
	int bytes_in_pixel;
	BYTE *data;
	int data_size;
};

bool IL_LoadImage(WCHAR *filename, IMAGE_INFO *data_info);

class TEXTURE
{
public:
	GLuint texture;
	IMAGE_INFO ImageInfo;

	bool CreateTextureFromFile(WCHAR *filename);
	bool CreateTextureByParams(int size_x, int size_y, GLint internal_format, GLint format, GLint type, GLvoid *data, int set_array_num);
	void SetParams(int mag_filter, int min_filter, int wrap);
};

float *ActiveTextures(TEXTURE **Texture, int num);
void DeactiveTextures();

