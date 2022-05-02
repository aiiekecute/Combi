#include "stdafx.h"
#include "windows.h"
#include "math.h"

#include "shapes.h"

///////////SHAPE//////////////

void SHAPE::Init(SHAPES *set_Shapes, int set_this_index, int *set_data_pos, int set_data_num, SHADER *Shader)
{
	Shapes=set_Shapes;
	this_index=set_this_index;
	this_index_double=double(this_index);
	points_num=set_data_num;
	
	//create data
	int min_position[2]={10000,10000};
	int max_position[2]={0,0};
	for(int i=0; i<points_num; ++i)
	{
		min_position[0]=min(min_position[0], set_data_pos[i*2+0]);
		min_position[1]=min(min_position[1], set_data_pos[i*2+1]);
		max_position[0]=max(max_position[0], set_data_pos[i*2+0]);
		max_position[1]=max(max_position[1], set_data_pos[i*2+1]);
	}

	points_size[0]=max_position[0]-min_position[0]+1;
	points_size[1]=max_position[1]-min_position[1]+1;
	for(int x=0; x<points_size[0]; ++x) for(int y=0; y<points_size[1]; ++y) point_index[x][y]=-1;

	float temp_point_coords[50][50][2];

	int sum_position[2]={0,0};
	position_base[0]=0.0; position_base[1]=0.0;
	for(int i=0; i<points_num; ++i)
	{
		sum_position[0]+=set_data_pos[i*2+0]; sum_position[1]+=set_data_pos[i*2+1];

		int x=set_data_pos[i*2+0]-min_position[0]; int y=set_data_pos[i*2+1]-min_position[1];
		
		point_index[x][y]=i;
		temp_point_coords[x][y][0]=float(set_data_pos[i*2+0])+0.5f; temp_point_coords[x][y][1]=float(set_data_pos[i*2+1])+0.5f;
		point_dpos[x][y][0]=double(x)+0.5; point_dpos[x][y][1]=double(y)+0.5; 

		position_base[0]+=point_dpos[x][y][0]; position_base[1]+=point_dpos[x][y][1];
	}
	position_base[0]/=double(points_num); position_base[1]/=double(points_num);
	global_position[0]=float(sum_position[0])/float(points_num); global_position[1]=float(sum_position[1])/float(points_num);

	available_point_num=0;
	test_perimeter_num=0;
	for(int x=0; x<points_size[0]; ++x) for(int y=0; y<points_size[1]; ++y) if(point_index[x][y]>=0) 
	{
		point_dpos[x][y][0]=point_dpos[x][y][0]-position_base[0]; 
		point_dpos[x][y][1]=point_dpos[x][y][1]-position_base[1];

		point_position[x][y][0]=0.0+(point_dpos[x][y][0]*cos(0.0)+point_dpos[x][y][1]*cos(0.0+S_PI/2.0));
		point_position[x][y][1]=0.0+(point_dpos[x][y][0]*sin(0.0)+point_dpos[x][y][1]*sin(0.0+S_PI/2.0));

		available_point[available_point_num][0]=x;
		available_point[available_point_num][1]=y;
		++available_point_num;

		if( x==0 || y==0 || x==points_size[0]-1 || y==points_size[1]-1 || point_index[x-1][y]==-1 || point_index[x+1][y]==-1 || point_index[x][y-1]==-1 || point_index[x][y+1]==-1 ||  
			 point_index[x-1][y-1]==-1 || point_index[x+1][y-1]==-1 || point_index[x-1][y+1]==-1 || point_index[x+1][y+1]==-1 )
		{
			test_perimeter_point[test_perimeter_num][0]=x;
			test_perimeter_point[test_perimeter_num][1]=y;
			++test_perimeter_num;

			point_is_perimeter_flag[x][y]=true;
		}
		else
		{
			point_is_perimeter_flag[x][y]=false;
		}
	}

	for(int i=0; i<available_point_num; ++i)
	{
		int index1=rand()%available_point_num, index2=rand()%available_point_num;
		int temp[2]={available_point[index1][0], available_point[index1][1]};
		available_point[index1][0]=available_point[index2][0]; available_point[index1][1]=available_point[index2][1];
		available_point[index2][0]=temp[0]; available_point[index2][1]=temp[1];
	}

	//printf("----%d---%d----\n", test_perimeter_num, points_num);
	
	//set to null
	time=rand()%300;	
	is_active_flag=false;
	position_center[0]=0.0; position_center[1]=0.0;
	position_angle=0.0;
	impulse_position[0]=0.0; impulse_position[1]=0.0;
	impulse_angle=0.0;
	
	//create mesh
	int indexes[100000];
	float vertex_pos[10000][2];
	float vertex_texcoord[10000][2];
	for(int x=0; x<points_size[0]; ++x) for(int y=0; y<points_size[1]; ++y) if(point_index[x][y]>=0)
	{
		int index=point_index[x][y];
		vertex_pos[index][0]=point_position[x][y][0];
		vertex_pos[index][1]=point_position[x][y][1];
		vertex_texcoord[index][0]=temp_point_coords[x][y][0];
		vertex_texcoord[index][1]=temp_point_coords[x][y][1];
	}

	indexes_num=0; 
	for(int x=0; x<points_size[0]-1; ++x) for(int y=0; y<points_size[1]-1; ++y)
	{
		if(point_index[x][y]>=0 && point_index[x+1][y]>=0 && point_index[x][y+1]>=0 && point_index[x+1][y+1]>=0)
		{
			indexes[indexes_num++]=point_index[x][y];
			indexes[indexes_num++]=point_index[x+1][y];
			indexes[indexes_num++]=point_index[x][y+1];

			indexes[indexes_num++]=point_index[x+1][y];
			indexes[indexes_num++]=point_index[x+1][y+1];
			indexes[indexes_num++]=point_index[x][y+1];
		}
		else
		{
			int temp=0;
			if(point_index[x][y]>=0) indexes[indexes_num+temp++]=point_index[x][y];
			if(point_index[x+1][y]>=0) indexes[indexes_num+temp++]=point_index[x+1][y];
			if(point_index[x+1][y+1]>=0) indexes[indexes_num+temp++]=point_index[x+1][y+1];
			if(point_index[x][y+1]>=0) indexes[indexes_num+temp++]=point_index[x][y+1];
			if(temp==3) indexes_num+=3;
		}
	}

	Mesh.CreateVAOandVBO(2);

	glBindBuffer(GL_ARRAY_BUFFER, Mesh.vbo[0]);
	Mesh.CreateVerticesData(points_num, 2, (float*)vertex_pos, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Mesh.vbo[1]);
	Mesh.CreateVerticesData(points_num, 2, (float*)vertex_texcoord, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.index);
	Mesh.CreateIndexData(indexes_num, (int*)indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(Mesh.vao);
		glBindBuffer(GL_ARRAY_BUFFER, Mesh.vbo[0]);
			SetVertexAttrib(Shader->GetAttribIndex("position"), 2, 2, 0);
		glBindBuffer(GL_ARRAY_BUFFER, Mesh.vbo[1]);
			SetVertexAttrib(Shader->GetAttribIndex("texcoord"), 2, 2, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.index);
	glBindVertexArray(0);
}

void SHAPE::Simulate(int move_flag)
{
	if(move_flag==0)
	{
		//rotation
		++time;
		if(time<100) impulse_angle+=0.01;	
		else if(time<125) {}
		else if(time<225) impulse_angle-=0.01;
		else if(time<250) {}
		else time=0;
	}
	else if(move_flag==3)
	{
		//rotation
		++time;
		if(time<25) impulse_angle+=0.002;	
		else if(time<50) impulse_angle-=0.002;
		else time=0;
	}

	if(move_flag==0 || move_flag==1)
	{
		//distortion
		impulse_position[0]+=cos(Shapes->wave*1.0)*(move_flag==1 ? 0.001 : 0.3)+0.2*double(rand()%100-50)/300.0;	
		impulse_position[1]+=sin(Shapes->wave*1.0)*(move_flag==1 ? 0.001 : 0.3)+0.2*double(rand()%100-50)/300.0;
	}

	if(move_flag==3)
	{
		//distortion
		impulse_position[0]+=cos(Shapes->wave*1.0)*0.05+0.2*double(rand()%100-50)/700.0;	
		impulse_position[1]+=sin(Shapes->wave*1.0)*0.05+0.2*double(rand()%100-50)/700.0;
	}
			
	if(move_flag==1)
	{
		//move_from_center
		double dist=sqrt((position_center[0]-Shapes->field_center[0])*(position_center[0]-Shapes->field_center[0])+(position_center[1]-Shapes->field_center[1])*(position_center[1]-Shapes->field_center[1]))+0.0001;
		impulse_position[0]+=min(1.0,23.0/dist)*(position_center[0]-Shapes->field_center[0])/dist;	
		impulse_position[1]+=min(1.0,23.0/dist)*(position_center[1]-Shapes->field_center[1])/dist;
	}
	
	const int add_int[9][2]={{-1,-1},{0,-1},{+1,-1},{-1,0},{+1,0},{-1,+1},{0,+1},{+1,+1},{0,0}};
	double move_dist, move_vec[2];
	int add_x,add_y;
	double dx,dy;
	int ix,iy,field_index;
	double vec1_angle, vec2[2], vec2_dist, vec2_angle;
		
#define x available_point[ap][0]
#define y available_point[ap][1]
	//for(int x=0; x<points_size[0]; ++x) for(int y=0; y<points_size[1]; ++y) if(point_index[x][y]>=0)
	//double param_dist=(move_flag==2 ? 1.5 : 1.5), param_power=(move_flag==2 ? 0.35 : 0.35), param_angle=(move_flag==2 ? 0.01 : 0.01), param2_dist=param_dist*param_dist;
	const double param_dist=1.5, param_power=0.35, param_angle=0.01, param2_dist=1.5*1.5;

	for(int ap=0; ap<available_point_num; ++ap)
	{
		ix=int(point_position[x][y][0]); iy=int(point_position[x][y][1]);
		for(int i=0; i<9; ++i) 
		{
			add_x=ix+add_int[i][0]; add_y=iy+add_int[i][1];
			for(int j=0; j<Shapes->field_nums[add_x][add_y]; ++j) if( (j>0 && int(Shapes->field[add_x][add_y][j][2])!=this_index) || (j==0 && Shapes->field[add_x][add_y][j][2]==-1.0) )
			{
				dx=Shapes->field[add_x][add_y][j][0]-point_position[x][y][0]; dy=Shapes->field[add_x][add_y][j][1]-point_position[x][y][1];
				move_dist=dx*dx+dy*dy;
				if(move_dist<param2_dist) 
				{
					move_dist=sqrt(move_dist)+0.0001;

					//position
					dx=dx/move_dist; dy=dy/move_dist;
					move_dist=(param_dist-move_dist)*param_power;
					impulse_position[0]-=dx*move_dist; impulse_position[1]-=dy*move_dist;

					//angle
					vec1_angle=acos(-dx);
					if(-dy<0.0) vec1_angle=S_PI*2.0-vec1_angle;

					vec2[0]=point_position[x][y][0]-position_center[0]; vec2[1]=point_position[x][y][1]-position_center[1];
					vec2_dist=sqrt(vec2[0]*vec2[0]+vec2[1]*vec2[1])+0.0001;
					vec2[0]=vec2[0]/vec2_dist;	vec2[1]=vec2[1]/vec2_dist;
					vec2_angle=acos(vec2[0]);
					if(vec2[1]<0.0) vec2_angle=S_PI*2.0-vec2_angle;

					impulse_angle+=sin(vec1_angle-vec2_angle)*vec2_dist*move_dist*param_angle;
				}
			}
		}
	}
#undef x
#undef y
}

void SHAPE::UpdatePosition(bool is_first_update_flag)
{
	if(!is_first_update_flag)
	{
		impulse_position[0]*=0.91;
		impulse_position[1]*=0.91;
		impulse_angle*=0.975;
	
		double temp=sqrt(impulse_position[0]*impulse_position[0]+impulse_position[1]*impulse_position[1])+0.0001;
		if(temp>0.4) { impulse_position[0]=0.4*impulse_position[0]/temp; impulse_position[1]=0.4*impulse_position[1]/temp; }
	
		temp=fabs(impulse_angle)+0.0001;
		if(temp>0.1) { impulse_angle=0.1*impulse_angle/temp; }

		position_center[0]+=impulse_position[0];
		position_center[1]+=impulse_position[1];
		position_angle+=impulse_angle;
	}

	double precos[2]={cos(position_angle), cos(position_angle+S_PI/2.0)}; 
	double presin[2]={sin(position_angle), sin(position_angle+S_PI/2.0)};
	int ix,iy;

#define x available_point[ap][0]
#define y available_point[ap][1]
	//for(int x=0; x<points_size[0]; ++x) for(int y=0; y<points_size[1]; ++y) if(point_index[x][y]>=0)
	for(int ap=0; ap<available_point_num; ++ap)
	{
		point_position[x][y][0]=position_center[0]+point_dpos[x][y][0]*precos[0]+point_dpos[x][y][1]*precos[1];
		point_position[x][y][1]=position_center[1]+point_dpos[x][y][0]*presin[0]+point_dpos[x][y][1]*presin[1];

		ix=int(point_position[x][y][0]); iy=int(point_position[x][y][1]);

		if(Shapes->field_nums[ix][iy]<10 && (Shapes->field_nums[ix][iy]==1 || int(Shapes->field[ix][iy][ Shapes->field_nums[ix][iy]-1 ][2])!=this_index))
		{
			Shapes->field[ix][iy][ Shapes->field_nums[ix][iy] ][0]=point_position[x][y][0];
			Shapes->field[ix][iy][ Shapes->field_nums[ix][iy] ][1]=point_position[x][y][1];
			Shapes->field[ix][iy][ Shapes->field_nums[ix][iy] ][2]=double(this_index);
			++Shapes->field_nums[ix][iy];
		}
	}
#undef x
#undef y
}

inline bool SHAPE::Test(double *preangle)
{
	position_angle=preangle[4];
	int ixy[2];
	
#define x available_point[ap][0]
#define y available_point[ap][1]
	//for(int x=0; x<points_size[0]; ++x) for(int y=0; y<points_size[1]; ++y) if(point_index[x][y]>=0)
/*	for(int ap=0; ap<available_point_num; ++ap)
	{
		point_position[x][y][0]=position_center[0]+point_dpos[x][y][0]*preangle[0]+point_dpos[x][y][1]*preangle[1];
		point_position[x][y][1]=position_center[1]+point_dpos[x][y][0]*preangle[2]+point_dpos[x][y][1]*preangle[3];

		ixy[0]=int(point_position[x][y][0]); ixy[1]=int(point_position[x][y][1]);
		if(ixy[0]<0 || ixy[0]>=512 || ixy[1]<0 || ixy[1]>=512 || Shapes->field_nums[ixy[0]][ixy[1]]!=1 || Shapes->field[ixy[0]][ixy[1]][0][2]!=0.0 ) return false;
	}*/
		
	alignas(16) double stack_position_center[2]={position_center[0], position_center[1]};
	alignas(16) double stack_preangle_1[2]={preangle[0], preangle[2]};
	alignas(16) double stack_preangle_2[2]={preangle[1], preangle[3]};
	alignas(16) double stack_point_position[2];
	alignas(16) double stack_point_dpos_1[2], stack_point_dpos_2[2];
	
	for(int ap=0; ap<available_point_num; ++ap)
	{
		stack_point_dpos_1[0]=point_dpos[x][y][0]; stack_point_dpos_1[1]=point_dpos[x][y][0];
		stack_point_dpos_2[0]=point_dpos[x][y][1]; stack_point_dpos_2[1]=point_dpos[x][y][1];

		__asm 
		{
			movapd xmm0, stack_position_center;

			movapd xmm1, stack_point_dpos_1;
			movapd xmm2, stack_preangle_1;
			mulpd xmm1, xmm2;

			movapd xmm3, stack_point_dpos_2;
			movapd xmm4, stack_preangle_2;
			mulpd xmm3, xmm4;
						
			addpd xmm0, xmm1;
			addpd xmm0, xmm3;
			
			movapd stack_point_position, xmm0;
			
			cvtpd2pi mm0, xmm0;
			movq ixy, mm0;
		};
				
		if(/*ixy[0]<0 || ixy[0]>=512 || ixy[1]<0 || ixy[1]>=512 ||*/ Shapes->field_nums[ixy[0]][ixy[1]]!=1 || Shapes->field[ixy[0]][ixy[1]][0][2]!=0.0 ) return false;
		point_position[x][y][0]=stack_point_position[0]; point_position[x][y][1]=stack_point_position[1];
	}
	
#undef x
#undef y

	return true;
}

void SHAPE::SetFinishPosition(bool large_area_flag)
{
#define x available_point[ap][0]
#define y available_point[ap][1]
	//for(int x=0; x<points_size[0]; ++x) for(int y=0; y<points_size[1]; ++y) if(point_index[x][y]>=0)
	for(int ap=0; ap<available_point_num; ++ap)
	{
		int ix=int(point_position[x][y][0]); int iy=int(point_position[x][y][1]);
		for(int i=1; i<Shapes->field_nums[ix][iy]; ++i) if(int(Shapes->field[ix][iy][i][2])==this_index)
		{
			--Shapes->field_nums[ix][iy];
			Shapes->field[ix][iy][i][0]=Shapes->field[ix][iy][Shapes->field_nums[ix][iy]][0];
			Shapes->field[ix][iy][i][1]=Shapes->field[ix][iy][Shapes->field_nums[ix][iy]][1];
			Shapes->field[ix][iy][i][2]=Shapes->field[ix][iy][Shapes->field_nums[ix][iy]][2];
			--i;
		}
	}
#undef x
#undef y
	
	double best_variant_koef=CalculateFreeArea();

	int save_param_1, save_param_2; double *save_param_3;
	bool new_position_flag=false;

	double save_position_center[2]={position_center[0], position_center[1]};
	double save_position_angle=position_angle;

	int ix=int(position_center[0]), iy=int(position_center[1]);
	int step=(large_area_flag ? 15 : 8);
	for(int x=ix-step; x<=ix+step; ++x) for(int y=iy-step; y<=iy+step; ++y) if(/*x>=0 && x<512 && y>=0 && y<512 &&*/ Shapes->field_nums[x][y]==1 && Shapes->field[x][y][0][2]==0.0)
	{
		position_center[0]=double(x)+0.5; position_center[1]=double(y)+0.5; 
		for(int i=0; i<Shapes->preangle_num; ++i) if(Test(Shapes->preangle[i]))
		{
			double cur_variant_koef=CalculateFreeArea();
			if(cur_variant_koef>best_variant_koef)
			{
				new_position_flag=true;
				best_variant_koef=cur_variant_koef;
				save_param_1=x;
				save_param_2=y;
				save_param_3=&Shapes->preangle[i][0];
			}
		}
	}

	if(new_position_flag)
	{
		position_center[0]=double(save_param_1)+0.5; position_center[1]=double(save_param_2)+0.5; 
		Test(save_param_3);
		UpdatePosition(true);
	}
	else
	{
		position_center[0]=save_position_center[0]; position_center[1]=save_position_center[1];
		position_angle=save_position_angle;
		UpdatePosition(true);
	}
}

double SHAPE::CalculateFreeArea()
{
	int test_perimeter_counter=0;
	int x,y;
	for(int i=0; i<test_perimeter_num; ++i) 
	{
		x=point_position[ test_perimeter_point[i][0] ][ test_perimeter_point[i][1] ][0];
		y=point_position[ test_perimeter_point[i][0] ][ test_perimeter_point[i][1] ][1];

		if(		 Shapes->field[x+1][y][0][2]==-1.0 || Shapes->field_nums[x+1][y]>1 || Shapes->field[x-1][y][0][2]==-1.0 || Shapes->field_nums[x-1][y]>1 ||
				 Shapes->field[x][y+1][0][2]==-1.0 || Shapes->field_nums[x][y+1]>1 || Shapes->field[x][y-1][0][2]==-1.0 || Shapes->field_nums[x][y-1]>1)			test_perimeter_counter+=6;
		else if( Shapes->field[x-1][y-1][0][2]==-1.0 || Shapes->field_nums[x-1][y-1]>1 || Shapes->field[x+1][y-1][0][2]==-1.0 || Shapes->field_nums[x+1][y-1]>1 ||
				 Shapes->field[x-1][y+1][0][2]==-1.0 || Shapes->field_nums[x-1][y+1]>1 || Shapes->field[x+1][y+1][0][2]==-1.0 || Shapes->field_nums[x+1][y+1]>1)	test_perimeter_counter+=6;
		else if( Shapes->field[x+2][y][0][2]==-1.0 || Shapes->field_nums[x+2][y]>1 || Shapes->field[x-2][y][0][2]==-1.0 || Shapes->field_nums[x-2][y]>1 ||
				 Shapes->field[x][y+2][0][2]==-1.0 || Shapes->field_nums[x][y+2]>1 || Shapes->field[x][y-2][0][2]==-1.0 || Shapes->field_nums[x][y-2]>1)			test_perimeter_counter+=2;
		else if( Shapes->field[x-2][y+1][0][2]==-1.0 || Shapes->field_nums[x-2][y+1]>1 || Shapes->field[x-1][y+2][0][2]==-1.0 || Shapes->field_nums[x-1][y+2]>1 ||
				 Shapes->field[x+1][y+2][0][2]==-1.0 || Shapes->field_nums[x+1][y+2]>1 || Shapes->field[x+2][y+1][0][2]==-1.0 || Shapes->field_nums[x+2][y+1]>1 ||
				 Shapes->field[x-2][y-1][0][2]==-1.0 || Shapes->field_nums[x-2][y-1]>1 || Shapes->field[x-1][y-2][0][2]==-1.0 || Shapes->field_nums[x-1][y-2]>1 ||
				 Shapes->field[x+1][y-2][0][2]==-1.0 || Shapes->field_nums[x+1][y-2]>1 || Shapes->field[x+2][y-1][0][2]==-1.0 || Shapes->field_nums[x+2][y-1]>1)	test_perimeter_counter+=2;
		else if( Shapes->field[x-2][y-2][0][2]==-1.0 || Shapes->field_nums[x-2][y-2]>1 || Shapes->field[x+2][y-2][0][2]==-1.0 || Shapes->field_nums[x+2][y-2]>1 ||
				 Shapes->field[x-2][y+2][0][2]==-1.0 || Shapes->field_nums[x-2][y+2]>1 || Shapes->field[x+2][y+2][0][2]==-1.0 || Shapes->field_nums[x+2][y+2]>1)	test_perimeter_counter+=1;
	}

	return (double(test_perimeter_counter)/double(test_perimeter_num));
}

double SHAPE::CalculateDeleteKoef()
{
	int test_counter=0;
	int ix,iy,add_x,add_y;
	double dist;
	int max_add;

	const int add_int[9][2]={{-1,-1},{0,-1},{+1,-1},{-1,0},{+1,0},{-1,+1},{0,+1},{+1,+1},{0,0}};

#define x available_point[ap][0]
#define y available_point[ap][1]
	for(int ap=0; ap<available_point_num; ++ap) 
	{
		max_add=0;
		ix=int(point_position[x][y][0]); iy=int(point_position[x][y][1]);

		for(int i=0; i<9; ++i) 
		{
			add_x=ix+add_int[i][0]; add_y=iy+add_int[i][1];
			for(int j=0; j<Shapes->field_nums[add_x][add_y]; ++j)
			{
				if( (j==0 && Shapes->field[add_x][add_y][j][2]==-1.0) || (j>0 && int(Shapes->field[add_x][add_y][j][2])!=this_index) )
				{
					dist=sqrt( (point_position[x][y][0]-Shapes->field[add_x][add_y][j][0])*(point_position[x][y][0]-Shapes->field[add_x][add_y][j][0]) + (point_position[x][y][1]-Shapes->field[add_x][add_y][j][1])*(point_position[x][y][1]-Shapes->field[add_x][add_y][j][1]) );
					if(dist<0.7 && !point_is_perimeter_flag[x][y]) {max_add=10; goto ZZ;}
				//	else if(dist<0.3 && point_is_perimeter_flag[x][y]) {max_add=1;} 
				}
			}
		}
	
		ZZ:
		test_counter+=max_add;
	}
#undef x
#undef y

	return double(test_counter)/( ( double(test_perimeter_num)+sqrt(double(test_perimeter_num)) )/2.0 );
}

void SHAPE::SaveBestPosition()
{
	save_is_active_flag=is_active_flag;
	save_position_center[0]=position_center[0];
	save_position_center[1]=position_center[1];
	save_position_angle=position_angle;
}

void SHAPE::Restart()
{
	is_active_flag=false;
	time=rand()%300;	
	position_center[0]=0.0; position_center[1]=0.0;
	position_angle=0.0;
	impulse_position[0]=0.0; impulse_position[1]=0.0;
	impulse_angle=0.0;
}

///////////SHAPES//////////////

void SHAPES::Init(int set_thread_id, SHADER *Shader, SHADER *FieldShader)
{
	srand(timeGetTime());

	del_index=-1;
	thread_id=set_thread_id;
	best_parameter=INT_MAX;
	global_best_parameter=INT_MAX;

	Texture.CreateTextureFromFile(L"textures\\texture.bmp");

	mass_indexes=new bool*[Texture.ImageInfo.size_x];
	for(int i=0; i<Texture.ImageInfo.size_x; ++i) mass_indexes[i]=new bool[Texture.ImageInfo.size_y];
	for(int x=0; x<Texture.ImageInfo.size_x; ++x) for(int y=0; y<Texture.ImageInfo.size_y; ++y)
	{
		BYTE *pix=&Texture.ImageInfo.data[ (y*Texture.ImageInfo.size_x+x)*Texture.ImageInfo.bytes_in_pixel ];
		mass_indexes[x][y]=(pix[0]==255 && pix[1]==0 && pix[2]==255);
	}
		
	data_pos=new int[10000*2];
	
	Shapes_num=0;
	for(int y=0; y<Texture.ImageInfo.size_y; ++y) for(int x=0; x<Texture.ImageInfo.size_x; ++x) if(!mass_indexes[x][y])
	{
		data_num=0;
		Rec(x,y);
		if(data_num>1) 
		{
			Shape[Shapes_num].Init(this, Shapes_num, data_pos, data_num, Shader);
			++Shapes_num;
		}
	}
	printf("Shapes_num=%d\n",Shapes_num);

	delete [] data_pos;
	for(int i=0; i<Texture.ImageInfo.size_x; ++i) delete [] mass_indexes[i];
	delete [] mass_indexes;

	/////////////////////////////////////

	TextureField.CreateTextureFromFile(L"textures\\texture_field.bmp");

	MeshField.CreateVAOandVBO(1);

	float vertexes[]=
	{ 
		0.0, 0.0, 0.0, 0.0,
		512.0, 0.0, 1.0, 0.0,
		0.0, 512.0, 0.0, 1.0,
		512.0, 512.0, 1.0, 1.0
	};

	glBindBuffer(GL_ARRAY_BUFFER, MeshField.vbo[0]);
	MeshField.CreateVerticesData(4, 4, vertexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(MeshField.vao);
		glBindBuffer(GL_ARRAY_BUFFER, MeshField.vbo[0]);
			SetVertexAttrib(FieldShader->GetAttribIndex("position"), 2, 4, 0);
			SetVertexAttrib(FieldShader->GetAttribIndex("texcoord"), 2, 4, 2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
		
	int temp_num=0;
	field_center[0]=0.0; field_center[1]=0.0;
	for(int x=0; x<512; ++x) for(int y=0; y<512; ++y) 
	{
		field_nums[x][y]=1;
		field[x][y][0][0]=double(x)+0.5;
		field[x][y][0][1]=double(y)+0.5;
		
		BYTE *pix=&TextureField.ImageInfo.data[ (y*TextureField.ImageInfo.size_x+x)*TextureField.ImageInfo.bytes_in_pixel ];
		if(pix[0]==0 && pix[1]==0 && pix[2]==0) 
		{
			field[x][y][0][2]=0.0;
			field_center[0]+=field[x][y][0][0];
			field_center[1]+=field[x][y][0][1];
			++temp_num;
		}
		else 
		{
			field[x][y][0][2]=-1.0;
		}
	}
	field_center[0]/=double(temp_num); field_center[1]/=double(temp_num);
	view_field_center[0]=field_center[0]; view_field_center[1]=field_center[1];
	
	field_available_num=0;
	for(int x=0; x<512; ++x) for(int y=0; y<512; ++y) if(field[x][y][0][2]==0.0)
	{
		field_available[field_available_num][0]=x; field_available[field_available_num][1]=y; ++field_available_num;
	}

	preangle_num=0;
	for(double angle=0.0; angle<S_PI*2.0; angle+=0.02)
	{
		preangle[preangle_num][0]=cos(angle);
		preangle[preangle_num][1]=cos(angle+S_PI/2.0);
		preangle[preangle_num][2]=sin(angle);
		preangle[preangle_num][3]=sin(angle+S_PI/2.0);
		preangle[preangle_num][4]=angle;
		++preangle_num;
	}

	wave=0.0;
}

void SHAPES::Rec(int x, int y)
{
	mass_indexes[x][y]=true;
	data_pos[ data_num*2+0 ]=x;
	data_pos[ data_num*2+1 ]=y;
	++data_num;
	
	if(x+1<Texture.ImageInfo.size_x && !mass_indexes[x+1][y]) Rec(x+1,y);
	if(x-1>=0 && !mass_indexes[x-1][y]) Rec(x-1,y);
	if(y+1<Texture.ImageInfo.size_y && !mass_indexes[x][y+1]) Rec(x,y+1);
	if(y-1>=0 && !mass_indexes[x][y-1]) Rec(x,y-1);
}

void SHAPES::Simulate(int move_flag)
{
	wave+=0.03;

	for(int i=0; i<Shapes_num; ++i) if(Shape[i].is_active_flag)
	{
		Shape[i].Simulate(move_flag);
	}
}

void SHAPES::UpdatePosition()
{
	memset(field_nums,1,512*512);

	for(int i=0; i<Shapes_num; ++i) if(Shape[i].is_active_flag)
	{
		Shape[i].UpdatePosition(false);
		//if(i==0) printf("%f\n",Shape[i].position_center[0]);
	}
}

bool SHAPES::Test(int counter)
{
	SHAPE *pShape[200];
	for(int i=0; i<Shapes_num; ++i) pShape[i]=&Shape[i];
	if(counter==0)
	{
		for(int i=0; i<Shapes_num; ++i)
		{
			int index1=rand()%Shapes_num, index2=rand()%Shapes_num;
			SHAPE *temp_pShape=pShape[index1];
			pShape[index1]=pShape[index2];
			pShape[index2]=temp_pShape;
		}
	}
	else
	{
		for(int i=0; i<Shapes_num-1; ++i) for(int j=i+1; j<Shapes_num; ++j) if(pShape[j]->points_num > pShape[i]->points_num)
		{
			SHAPE *temp_pShape=pShape[i];
			pShape[i]=pShape[j];
			pShape[j]=temp_pShape;
		}
	}
	
	double best_variant_koef=-1.0, cur_variant_koef;
	int save_index, save_param_1, save_param_2; double *save_param_3;
	
	for(int i=0; i<Shapes_num; ++i) if(!pShape[i]->is_active_flag) 
	{
		bool flag=true;//(counter>0 || i%2==0);
		int j=0;//(flag ? 0 : field_available_num-1);
		int plus=1;//(flag ? +1 : -1);

		while( (flag && j<field_available_num) || (!flag && j>=0) )
		{
			if(field_nums[field_available[j][0]][field_available[j][1]]==1) 
			{
				pShape[i]->position_center[0]=double(field_available[j][0])+0.5; pShape[i]->position_center[1]=double(field_available[j][1])+0.5; 

				for(int k=0; k<preangle_num; ++k) if(pShape[i]->Test(preangle[k]))
				{
					if(counter==0)
					{
						pShape[i]->is_active_flag=true;
						pShape[i]->UpdatePosition(true);
						pShape[i]->SetFinishPosition(false); 
						goto BB;
					}
					else
					{
						cur_variant_koef=pShape[i]->CalculateFreeArea();
						if(cur_variant_koef>best_variant_koef)
						{
							best_variant_koef=cur_variant_koef;
							save_index=i;
							save_param_1=field_available[j][0];
							save_param_2=field_available[j][1];
							save_param_3=&preangle[k][0];
						}
					}
				}			
			}
			j+=plus;
		}
				
		if(counter>0 && best_variant_koef>-1.0)
		{
			pShape[save_index]->position_center[0]=double(save_param_1)+0.5; pShape[save_index]->position_center[1]=double(save_param_2)+0.5; 
			pShape[save_index]->Test(save_param_3);
			pShape[save_index]->is_active_flag=true;
			pShape[save_index]->UpdatePosition(true);
			return true;
		}
		BB:;
	}

	return false;
}

void SHAPES::SetFinishPosition()
{
	SHAPE *pShape[200];
	for(int i=0; i<Shapes_num; ++i) pShape[i]=&Shape[i];
	for(int i=0; i<Shapes_num-1; ++i) for(int j=i+1; j<Shapes_num; ++j) if(pShape[j]->points_num < pShape[i]->points_num)
	{
		SHAPE *temp_pShape=pShape[i];
		pShape[i]=pShape[j];
		pShape[j]=temp_pShape;
	}

	for(int i=0; i<Shapes_num; ++i) if(pShape[i]->is_active_flag) pShape[i]->SetFinishPosition(true); 
}

int SHAPES::CalculateShapeForDelete()
{
	double best_param=0.0, cur_param;
	int best_index=-1;

	for(int i=0; i<Shapes_num; ++i) if(Shape[i].is_active_flag)
	{
		cur_param=Shape[i].CalculateDeleteKoef();
		if(cur_param>1.7 && cur_param>best_param)
		{
			best_param=cur_param;
			best_index=i;
		}
	}

	return best_index;
}

void SHAPES::SetFieldCenter()
{
	int max_circle_diameter=0;
	for(int i=0; i<Shapes_num; ++i) if(!Shape[i].is_active_flag) max_circle_diameter=max(max_circle_diameter, max(Shape[i].points_size[0],Shape[i].points_size[1]));

	int circle_mass[10000][2];
	double circle_mass_koef[10000];
	int circle_mass_num=0;
	for(int x=-max_circle_diameter/2; x<=max_circle_diameter/2; ++x) for(int y=-max_circle_diameter/2; y<=max_circle_diameter/2; ++y)
	{
		double dist=sqrt(double(x*x+y*y));
		double max_dist=double(max_circle_diameter)/2.0;
		if(dist < max_dist) 
		{
			circle_mass[circle_mass_num][0]=x;
			circle_mass[circle_mass_num][1]=y;
			circle_mass_koef[circle_mass_num]=1.0-dist/max_dist; circle_mass_koef[circle_mass_num]*=circle_mass_koef[circle_mass_num];
			++circle_mass_num;
		}
	}

	int save_field_center[2]={int(field_center[0]),int(field_center[1])};
	double max_free_space=0.0, cur_free_space;
	int x,y;
	for(int i=0; i<field_available_num; ++i) if(field_nums[field_available[i][0]][field_available[i][1]]==1 &&
		sqrt( double((save_field_center[0]-field_available[i][0])*(save_field_center[0]-field_available[i][0])+(save_field_center[1]-field_available[i][1])*(save_field_center[1]-field_available[i][1])) ) > double(max_circle_diameter)/2.0 )
	{
		cur_free_space=0.0;
		for(int j=0; j<circle_mass_num; ++j) 
		{
			x=field_available[i][0]+circle_mass[j][0];
			y=field_available[i][1]+circle_mass[j][1];
			if(x>=0+1 && x<512-1 && y>=0+1 && y<512-1 && field_nums[x][y]==1 && field[x][y][0][2]==0.0) 
			{
				if( ((field_nums[x-1][y]>1 || field[x-1][y][0][2]!=0.0) && (field_nums[x+1][y]>1 || field[x+1][y][0][2]!=0.0)) ||
					((field_nums[x][y-1]>1 || field[x][y-1][0][2]!=0.0) && (field_nums[x][y+1]>1 || field[x][y+1][0][2]!=0.0)) ||
					((field_nums[x-1][y-1]>1 || field[x-1][y-1][0][2]!=0.0) && (field_nums[x+1][y+1]>1 || field[x+1][y+1][0][2]!=0.0)) ||
					((field_nums[x-1][y+1]>1 || field[x-1][y+1][0][2]!=0.0) && (field_nums[x+1][y-1]>1 || field[x+1][y-1][0][2]!=0.0)) )
				{
					cur_free_space+=0.0;
				}
				else
				{
					double koef=1.0;
					if(field[x-1][y][0][2]==-1.0 || field[x+1][y][0][2]==-1.0 || field[x][y-1][0][2]==-1.0 || field[x][y+1][0][2]==-1.0 ||
					   field[x-1][y-1][0][2]==-1.0 || field[x+1][y-1][0][2]==-1.0 || field[x-1][y+1][0][2]==-1.0 || field[x+1][y+1][0][2]==-1.0) koef=0.1;
					cur_free_space+=circle_mass_koef[j]*koef;
				}
			}
		}
		if(cur_free_space>max_free_space)
		{
			field_center[0]=double(field_available[i][0])+0.5;
			field_center[1]=double(field_available[i][1])+0.5;
			max_free_space=cur_free_space;
		}
	}

//	printf("center= %f %f %d\n", field_center[0], field_center[1], max_free_space);
}

bool SHAPES::CalculateBestParameter()
{
	int current_best_parameter=0;//low-better, high-badly

	for(int x=0; x<512-2; ++x) for(int y=0; y<512-2; ++y) 
	{
		for(int dx=0; dx<2; ++dx) for(int dy=0; dy<2; ++dy) if(field_nums[x+dx][y+dy]!=1 || field[x+dx][y+dy][0][2]!=0.0) goto A; 
		current_best_parameter+=2;
		A:;
	}
	for(int x=0; x<512-2; ++x) for(int y=0; y<512-3; ++y) 
	{
		for(int dx=0; dx<2; ++dx) for(int dy=0; dy<3; ++dy) if(field_nums[x+dx][y+dy]!=1 || field[x+dx][y+dy][0][2]!=0.0) goto AA; 
		current_best_parameter+=15;
		AA:;
	}
	for(int x=0; x<512-3; ++x) for(int y=0; y<512-2; ++y) 
	{
		for(int dx=0; dx<3; ++dx) for(int dy=0; dy<2; ++dy) if(field_nums[x+dx][y+dy]!=1 || field[x+dx][y+dy][0][2]!=0.0) goto AAA; 
		current_best_parameter+=15;
		AAA:;
	}

	for(int x=0; x<512-3; ++x) for(int y=0; y<512-3; ++y) 
	{
		for(int dx=0; dx<3; ++dx) for(int dy=0; dy<3; ++dy) if(field_nums[x+dx][y+dy]!=1 || field[x+dx][y+dy][0][2]!=0.0) goto B; 
		current_best_parameter+=45;
		B:;
	}
	for(int x=0; x<512-4; ++x) for(int y=0; y<512-4; ++y) 
	{
		for(int dx=0; dx<4; ++dx) for(int dy=0; dy<4; ++dy) if(field_nums[x+dx][y+dy]!=1 || field[x+dx][y+dy][0][2]!=0.0) goto C; 
		current_best_parameter+=130;
		C:;
	}
	for(int x=0; x<512-5; ++x) for(int y=0; y<512-5; ++y) 
	{
		for(int dx=0; dx<5; ++dx) for(int dy=0; dy<5; ++dy) if(field_nums[x+dx][y+dy]!=1 || field[x+dx][y+dy][0][2]!=0.0) goto D; 
		current_best_parameter+=270;
		D:;
	}

	int add_parameter=0;

	for(int x=1; x<512-1; ++x) 
	{
		for(int y=1; y<512-1; ++y)
		{
			int cur_max=0;
			for(int i=1; i<field_nums[x][y]; ++i) 
			{
				double this_x=field[x][y][i][0];
				double this_y=field[x][y][i][1];
				int this_index=int(field[x][y][i][2]);
			
				bool temp_flag=false;
				for(int xx=0; xx<Shape[this_index].points_size[0]; ++xx) for(int yy=0; yy<Shape[this_index].points_size[1]; ++yy) if(Shape[this_index].point_index[xx][yy]>=0) 
				{
					if(this_x==Shape[this_index].point_position[xx][yy][0] && this_y==Shape[this_index].point_position[xx][yy][1] && Shape[this_index].point_is_perimeter_flag[xx][yy]) { temp_flag=true; break; }
				}

				for(int jx=x-1; jx<=x+1; ++jx) for(int jy=y-1; jy<=y+1; ++jy) for(int j=0; j<field_nums[jx][jy]; ++j)
				{
					if((j==0 && field[jx][jy][j][2]==-1.0) || (j>0 && int(field[jx][jy][j][2])!=this_index))
					{
						double dist=sqrt( (this_x-field[jx][jy][j][0])*(this_x-field[jx][jy][j][0])+(this_y-field[jx][jy][j][1])*(this_y-field[jx][jy][j][1]) );
						if(dist<0.7 && !temp_flag) {cur_max=150; goto G;}
						else if(dist<0.1 && temp_flag) {cur_max=1;}
					}
				}
			}
		G:;
			add_parameter+=cur_max;
		}
	}

	current_best_parameter+=(add_parameter/5);
	
	if(current_best_parameter<=best_parameter)
	{
		printf("--Thread=%d, New=%d, Prev=%d, Global=%d--\n", thread_id, current_best_parameter, best_parameter, global_best_parameter);
		best_parameter=current_best_parameter;
		return true;
	}
	else
	{
		printf("thread=%d, current=%d, best=%d, global=%d\n", thread_id, current_best_parameter, best_parameter, global_best_parameter);
		return false;
	}
}

void SHAPES::SaveBestPosition()
{
	for(int i=0; i<Shapes_num; ++i)
	{
		Shape[i].SaveBestPosition();
	}
}

void SHAPES::Restart()
{
	srand(timeGetTime());
	del_index=-1;
	best_parameter=INT_MAX;
	wave=double(rand()%1000)/100.0;
	memset(field_nums,1,512*512);

	for(int i=0; i<Shapes_num; ++i)
	{
		Shape[i].Restart();
	}
}