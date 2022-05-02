#include "stdafx.h"
#include "windows.h"
#include "math.h"

#include	<GL/glew.h>
#include	<GL/wglew.h>

#include	<GL/freeglut.h>

#include "tools.h"
#include "shader.h"
#include "geometry.h"
#include "texture.h"
#include "framebuffer.h"

#include "shapes.h"

#include <IL/il.h>
#include <IL/ilu.h>

#define THREADS_NUM 6

FRAMEBUFFER TempFramebuffer;

TEXTURE BoxTexture, NumbersTexture, InputDataTexture;
MESH_DATA InputDataMesh, NumbersMesh;
SHADER BoxShader, FieldShader, InputDataShader, NumbersShader;

SHAPES Shapes[THREADS_NUM];

int time, times;
float average_time_float;

void DrawField(int best_index)
{
	TEXTURE *tex[1]={&Shapes[best_index].TextureField};
	float *act_tex=ActiveTextures(tex,1);

	glUseProgram(FieldShader.program);
		
		FieldShader.SetUniform("Texture",&act_tex[0],UINT1);

		float view_center[2]={Shapes[best_index].view_field_center[0], Shapes[best_index].view_field_center[1]};
		float free_point[2]={Shapes[best_index].field_center[0], Shapes[best_index].field_center[1]};
		FieldShader.SetUniform("view_center", view_center, UFLOAT2);
		
		glBindVertexArray(Shapes[best_index].MeshField.vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

	glUseProgram(0);

	DeactiveTextures();

}

void DrawBox(int best_index)
{
	TEXTURE *tex[3]={&BoxTexture, &Shapes[best_index].Texture, &NumbersTexture};
	float *act_tex=ActiveTextures(tex,3);

	glUseProgram(BoxShader.program);
			
		BoxShader.SetUniform("boxTexture",&act_tex[0],UINT1);
		BoxShader.SetUniform("alphaTexture",&act_tex[1],UINT1);
		BoxShader.SetUniform("numbersTexture",&act_tex[2],UINT1);
	
		for(int i=0; i<Shapes[best_index].Shapes_num; ++i) if(Shapes[best_index].Shape[i].save_is_active_flag)
		{
		//	printf("%d = %d\n",i,Shapes[best_index].Shape[i].points_num);

			float position[3]={Shapes[best_index].Shape[i].save_position_center[0], Shapes[best_index].Shape[i].save_position_center[1], Shapes[best_index].Shape[i].save_position_angle};
			float view_center[2]={Shapes[best_index].view_field_center[0], Shapes[best_index].view_field_center[1]};
			float index_texcoord_center[2]={(float(int(i/30))+0.5)/10.0, 1.0-(float(int(i%30))+0.5)/30.0};

		//	printf("%f %f\n",index_texcoord_center[0],index_texcoord_center[1]);

			BoxShader.SetUniform("point_pos", position, UFLOAT3);
			BoxShader.SetUniform("view_center", view_center, UFLOAT2);
			BoxShader.SetUniform("index_texcoord_center", index_texcoord_center, UFLOAT2);

			glBindVertexArray(Shapes[best_index].Shape[i].Mesh.vao);
				glDrawElements(GL_TRIANGLES, Shapes[best_index].Shape[i].indexes_num, GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
		}

	/*	for(int i=0; i<Shapes[best_index].Shapes_num; ++i) if(Shapes[best_index].Shape[i].is_active_flag)
		{
			float position[3]={Shapes[best_index].Shape[i].position_center[0], Shapes[best_index].Shape[i].position_center[1], Shapes[best_index].Shape[i].position_angle};
			float view_center[2]={Shapes[best_index].view_field_center[0], Shapes[best_index].view_field_center[1]};
			float kk=(Shapes[best_index].del_index==Shapes[best_index].Shape[i].this_index ? 1.0 : 0.0);

			BoxShader.SetUniform("point_pos", position, UFLOAT3);
			BoxShader.SetUniform("view_center", view_center, UFLOAT2);
			BoxShader.SetUniform("kk", &kk, UFLOAT1);

			glBindVertexArray(Shapes[best_index].Shape[i].Mesh.vao);
				glDrawElements(GL_TRIANGLES, Shapes[best_index].Shape[i].indexes_num, GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0);
		}*/

	glUseProgram(0);

	DeactiveTextures();
}

void DrawInputData()
{
	TEXTURE *tex[1]={&InputDataTexture};
	float *act_tex=ActiveTextures(tex,1);

	glUseProgram(InputDataShader.program);
		
		InputDataShader.SetUniform("Texture",&act_tex[0],UINT1);
		
		glBindVertexArray(InputDataMesh.vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

	glUseProgram(0);

	DeactiveTextures();
}

void DrawNumbers()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	TEXTURE *tex[1]={&NumbersTexture};
	float *act_tex=ActiveTextures(tex,1);

	glUseProgram(NumbersShader.program);
		
		NumbersShader.SetUniform("Texture",&act_tex[0],UINT1);
		
		for(int i=0; i<Shapes[0].Shapes_num; ++i)
		{
			float position[2]={Shapes[0].Shape[i].global_position[0]/512.0, Shapes[0].Shape[i].global_position[1]/512.0};
			float index_texcoord_center[2]={(float(int(i/30))+0.5)/10.0, 1.0-(float(int(i%30))+0.5)/30.0};

			NumbersShader.SetUniform("point_pos", position, UFLOAT2);
			NumbersShader.SetUniform("index_texcoord_center", index_texcoord_center, UFLOAT2);

			glBindVertexArray(NumbersMesh.vao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}

	glUseProgram(0);

	DeactiveTextures();

	glDisable(GL_BLEND);
}

void display()
{
	static bool is_first_run=true;
	if(is_first_run)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, TempFramebuffer.id);
		glViewport(0,0,2048,2048);
		glClear(GL_COLOR_BUFFER_BIT);

		DrawInputData();
		DrawNumbers();

		BYTE *pixels_data=new BYTE[2048*2048*3];
		glReadPixels(0, 0, 2048, 2048, GL_RGB, GL_UNSIGNED_BYTE, pixels_data);

		ilEnable(IL_FILE_OVERWRITE); 
		ILuint handle;
		ilGenImages(1,&handle);
		ilBindImage(handle);
		ilTexImage(2048, 2048, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, pixels_data);
		ilSaveImage(L"textures\\init_pic.bmp");
		ilDeleteImages(1, &handle);
				
		delete [] pixels_data;

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		TempFramebuffer.Delete();

		is_first_run=false;
	}
	else
	{
		glViewport(0,0,1024,1024);
		glClear(GL_COLOR_BUFFER_BIT);

		static int static_best_index=-1;
		int best_index=0;
		for(int i=0; i<THREADS_NUM; ++i) if(Shapes[i].global_best_parameter<=Shapes[best_index].global_best_parameter) best_index=i;
		if(static_best_index!=best_index) 
		{
			static_best_index=best_index;
			printf("Draw thread index: %d\n",best_index);
		}

		DrawField(best_index);
		if(Shapes[best_index].global_best_parameter < INT_MAX) DrawBox(best_index);
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void LoadFramebuffers()
{
	TempFramebuffer.Create(2048, 2048, GL_RGBA8, NULL, NULL);
	TempFramebuffer.AttachRenderbuffer(GL_COLOR_ATTACHMENT0,TempFramebuffer.renderbuffer_color_id);
}

void LoadBoxData()
{
	//SHADER
	GLuint box_shader_vs, box_shader_fs;
	CompileShader(&box_shader_vs, "box_vs.c", GL_VERTEX_SHADER);
	CompileShader(&box_shader_fs, "box_fs.c", GL_FRAGMENT_SHADER);
	BoxShader.CreateProgram(box_shader_vs, box_shader_fs);

	GLuint field_shader_vs, field_shader_fs;
	CompileShader(&field_shader_vs, "field_vs.c", GL_VERTEX_SHADER);
	CompileShader(&field_shader_fs, "field_fs.c", GL_FRAGMENT_SHADER);
	FieldShader.CreateProgram(field_shader_vs, field_shader_fs);

	GLuint inputdata_shader_vs, inputdata_shader_fs;
	CompileShader(&inputdata_shader_vs, "inputdata_vs.c", GL_VERTEX_SHADER);
	CompileShader(&inputdata_shader_fs, "inputdata_fs.c", GL_FRAGMENT_SHADER);
	InputDataShader.CreateProgram(inputdata_shader_vs, inputdata_shader_fs);

	GLuint numbers_shader_vs, numbers_shader_fs;
	CompileShader(&numbers_shader_vs, "numbers_vs.c", GL_VERTEX_SHADER);
	CompileShader(&numbers_shader_fs, "numbers_fs.c", GL_FRAGMENT_SHADER);
	NumbersShader.CreateProgram(numbers_shader_vs, numbers_shader_fs);

	//TEXTURE
	BoxTexture.CreateTextureFromFile(L"textures\\facture.bmp");
	glBindTexture(GL_TEXTURE_2D, BoxTexture.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	NumbersTexture.CreateTextureFromFile(L"textures\\numbers.bmp");
	glBindTexture(GL_TEXTURE_2D, NumbersTexture.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);

	InputDataTexture.CreateTextureFromFile(L"textures\\texture.bmp");
	glBindTexture(GL_TEXTURE_2D, InputDataTexture.texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	//MESH
	InputDataMesh.CreateVAOandVBO(1);
	float vertexes1[]=
	{ 
		0.0,  0.0,  0.0,  0.0,
		1.0,  0.0,  1.0,  0.0,
		0.0,  1.0,  0.0,  1.0,
		1.0,  1.0,  1.0,  1.0
	};
	glBindBuffer(GL_ARRAY_BUFFER, InputDataMesh.vbo[0]);
	InputDataMesh.CreateVerticesData(4, 4, vertexes1, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(InputDataMesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, InputDataMesh.vbo[0]);
			SetVertexAttrib(InputDataShader.GetAttribIndex("position"), 2, 4, 0);
			SetVertexAttrib(InputDataShader.GetAttribIndex("texcoord"), 2, 4, 2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	NumbersMesh.CreateVAOandVBO(1);
	float vertexes2[]=
	{ 
		-1.0,  -1.0,  -1.0,  -1.0,
		+1.0,  -1.0,  +1.0,  -1.0,
		-1.0,  +1.0,  -1.0,  +1.0,
		+1.0,  +1.0,  +1.0,  +1.0
	};
	glBindBuffer(GL_ARRAY_BUFFER, NumbersMesh.vbo[0]);
	NumbersMesh.CreateVerticesData(4, 4, vertexes2, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(NumbersMesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, NumbersMesh.vbo[0]);
			SetVertexAttrib(NumbersShader.GetAttribIndex("position"), 2, 4, 0);
			SetVertexAttrib(NumbersShader.GetAttribIndex("texcoord"), 2, 4, 2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
	srand(timeGetTime());

	int index=*((int*) lpParam);
	int counter=0;
	int best_parameter_counter=0;

	while(true)
	{
		if(counter>0)
		{
			for(int i=0; i<600; ++i)
			{
				Shapes[index].Simulate(1);
				Shapes[index].UpdatePosition();
			}
			Shapes[index].SetFinishPosition();
		}

		while(Shapes[index].Test(counter)) {}

		if(counter==0) Shapes[index].SetFinishPosition();

		for(int i=0; i<900; ++i)
		{
			Shapes[index].Simulate(0);
			Shapes[index].UpdatePosition();
		}

		for(int i=0; i<200; ++i)
		{
			Shapes[index].Simulate(2);
			Shapes[index].UpdatePosition();
		}
		
		do
		{
			Shapes[index].SetFinishPosition();

			Shapes[index].del_index=Shapes[index].CalculateShapeForDelete();
			if(Shapes[index].del_index>=0)
			{
				Shapes[index].Shape[ Shapes[index].del_index ].is_active_flag=false;
				Shapes[index].UpdatePosition();
				while(Shapes[index].Test(counter)) {}
			}

			for(int i=0; i<200; ++i)
			{
				Shapes[index].Simulate(3);
				Shapes[index].UpdatePosition();
			}
			for(int i=0; i<200; ++i)
			{
				Shapes[index].Simulate(2);
				Shapes[index].UpdatePosition();
			}
		}
		while(Shapes[index].del_index>=0);
			
		Shapes[index].SetFinishPosition();
		for(int i=0; i<200; ++i)
		{
			Shapes[index].Simulate(2);
			Shapes[index].UpdatePosition();
		}

		if(Shapes[index].CalculateBestParameter())
		{
			if(Shapes[index].best_parameter<=Shapes[index].global_best_parameter) 
			{
				Shapes[index].global_best_parameter=Shapes[index].best_parameter;
				Shapes[index].SaveBestPosition();
				printf("set global\n");
			}
			best_parameter_counter=0;
		}
		else
		{
			if(++best_parameter_counter>4)
			{
				counter=0;
				best_parameter_counter=0;

				Shapes[index].Restart();

				printf("Restart Thread=%d\n", Shapes[index].thread_id);
				
				continue;
			}
			printf("best_parameter_counter=%d\n", best_parameter_counter);
		}
		
		Shapes[index].SetFieldCenter();

		++counter;
	}
	
	return 0;
}

int _tmain(int argc, char* argv[])
{
	srand(timeGetTime());
	
	glutInit            ( &argc, argv );
    glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB );
    glutInitWindowSize  ( 1024, 1024 );
	glutInitWindowPosition(0,0);
    glutInitContextVersion ( 3, 3 );
    glutInitContextFlags   ( GLUT_CORE_PROFILE );
    glutInitContextProfile ( GLUT_FORWARD_COMPATIBLE );
    glutCreateWindow("Stoner");

	glewExperimental=GL_TRUE;
    glewInit();
    if(!GLEW_VERSION_3_3)
    {
        printf("OpenGL 3.3 not supported.\n");
		return 1;
    }

	LoadFramebuffers();
		
	LoadBoxData();
	
	for(int i=0; i<THREADS_NUM; ++i) Shapes[i].Init(i, &BoxShader, &FieldShader);

	DWORD dwThreadId[THREADS_NUM], dwThrdParam[THREADS_NUM];
	HANDLE hThread[THREADS_NUM];
	for(int i=0; i<THREADS_NUM; ++i)
	{
		dwThrdParam[i]=i;
		hThread[i]=CreateThread(NULL, 0, ThreadFunc, &dwThrdParam[i], 0, &dwThreadId[i]);   
	}

	wglSwapIntervalEXT(1);
	glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glutDisplayFunc(display);
	
	time=timeGetTime();
	times=0;
	average_time_float=0.0;

	glutMainLoop();	
	
	char w=0;
	scanf("%c",&w);

	return 0;
}

