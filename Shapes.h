#pragma once

#include "texture.h"
#include "geometry.h"
#include "shader.h"

#define S_PI 3.14159265359

class SHAPES;

class SHAPE
{
public:
	MESH_DATA Mesh;
	SHAPES *Shapes;
	int this_index;
	double this_index_double;
	bool is_active_flag;

	int time;
	int point_index[50][50];
	double point_position[50][50][2];
	double point_dpos[50][50][2];
	bool point_is_perimeter_flag[50][50];
	int points_size[2];
	int points_num;
	int indexes_num;

	int available_point[50*50][2];
	int available_point_num;

	int test_perimeter_point[50*50][2];
	int test_perimeter_num;

	double position_base[2]; 
	double position_center[2];
	double position_angle;

	double impulse_position[2];
	double impulse_angle;

	bool save_is_active_flag;
	double save_position_center[2];
	double save_position_angle;

	float global_position[2];
	
	void Init(SHAPES *set_Shapes, int set_this_index, int *set_data_pos, int set_data_num, SHADER *Shader);
	void Simulate(int move_flag);
	void UpdatePosition(bool is_first_update_flag);
	inline bool Test(double *preangle);
	void SetFinishPosition(bool large_area_flag);
	double CalculateFreeArea();
	double CalculateDeleteKoef();
	void SaveBestPosition();
	void Restart();
};

class SHAPES
{
public:
	int thread_id;
	int best_parameter;
	int global_best_parameter;
	
	double field[512][512][10][3];
	unsigned char field_nums[512][512];
	double field_center[2];
	double view_field_center[2];
	
	int del_index;

	int field_available[512*512][2];
	int field_available_num;

	double preangle[1000][5];
	int preangle_num;

	SHAPE Shape[200];
	int Shapes_num;

	MESH_DATA MeshField;
	TEXTURE Texture, TextureField;

	bool **mass_indexes;
	int *data_pos;
	int data_num;

	double wave;

	void Init(int set_thread_id, SHADER *Shader, SHADER *FieldShader);
	void Rec(int x, int y);
	void Simulate(int move_flag);
	void UpdatePosition();
	bool Test(int counter);
	void SetFinishPosition();
	int CalculateShapeForDelete();
	void SetFieldCenter();
	bool CalculateBestParameter();
	void SaveBestPosition();
	void Restart();
};