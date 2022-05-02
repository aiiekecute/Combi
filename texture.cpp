#include "stdafx.h"
#include "windows.h"
#include "math.h"
#include "stdlib.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include "tools.h"
#include "texture.h"

bool IL_LoadImage(WCHAR *p_filename, IMAGE_INFO *data_info)
{
	static bool is_first_run=true;
	if(is_first_run)
	{
		ilInit();

		ilEnable(IL_ORIGIN_SET);
		is_first_run=false;
	}
	
	ILuint handle;
	ilGenImages(1,&handle);
	ilBindImage(handle);

		if( ilLoadImage(p_filename)!=IL_FALSE ) 
		{
			data_info->size_x=ilGetInteger(IL_IMAGE_WIDTH);
			data_info->size_y=ilGetInteger(IL_IMAGE_HEIGHT);

			data_info->bytes_in_pixel=3;//ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
			data_info->data_size=(data_info->size_x)*(data_info->size_y)*(data_info->bytes_in_pixel);
			data_info->data=new BYTE[data_info->data_size];
			int format;
			if(data_info->bytes_in_pixel==1) format=IL_ALPHA; else format=IL_BGR;

			ilCopyPixels(0, 0, 0, data_info->size_x, data_info->size_y, 1, format, IL_UNSIGNED_BYTE, data_info->data);
			ilDeleteImages(1, &handle);
			return true;
		}
	
/*	 int err = ilGetError();
     if (err != IL_NO_ERROR)
     {
         const wchar_t* strError = iluErrorString(err);
		 MessageBox(0,L"t",L"t",0);
       //  printf (strError);
     }*/

	ilDeleteImages(1, &handle);
	return false;
}

bool TEXTURE::CreateTextureFromFile(WCHAR *filename)
{
	if( !IL_LoadImage(filename, &ImageInfo) ) printf("Error, CreateTexture\n", NULL);
	
	glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, ImageInfo.size_x, ImageInfo.size_y, GL_BGR, GL_UNSIGNED_BYTE, (const GLvoid*) ImageInfo.data);
		glBindTexture(GL_TEXTURE_2D, 0);
		printf("create texture: %s\n",filename);

	return true;
}

bool TEXTURE::CreateTextureByParams(int size_x, int size_y, GLint internal_format, GLint format, GLint type, GLvoid *data, int set_array_num)
{
	printf("create texture by params\n", NULL);
				
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size_x, size_y, 0, format, type, data); 
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}

void TEXTURE::SetParams(int mag_filter, int min_filter, int wrap)
{
	int target=GL_TEXTURE_2D;

	glBindTexture(target, texture);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
}

///////////////////////////////
int ActiveTexturesNum=0;
TEXTURE *save_tex[10];

float *ActiveTextures(TEXTURE **Texture, int num_tex)
{
	static float act[10]={0,1,2,3,4,5,6,7,8,9};
	ActiveTexturesNum=num_tex;
	for(int i=0; i<num_tex; ++i)
	{
		save_tex[i]=Texture[i];
		glActiveTexture(GL_TEXTURE0+i);
    	glBindTexture(GL_TEXTURE_2D, Texture[i]->texture);
	}
	return act;
}
void DeactiveTextures()
{
	for(int i=ActiveTexturesNum-1; i>=0; --i)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
//////////////////////////////




