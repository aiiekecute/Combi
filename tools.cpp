#include "stdafx.h"
#include "windows.h"
#include "stdlib.h"
#include "string.h"

#include "tools.h"

void Tools_IntNumberToString(int num, char *str)
{
	int i=0;
	while(num>0)
	{
		str[i++]=48+num%10;
		num/=10;
	}
	int begin=0, end=i-1;
	char temp;
	while(begin<end) 
	{
		temp=str[begin];
		str[begin]=str[end];
		str[end]=temp;
		++begin;
		--end;
	}
	str[i]=0;
}

float Tools_StringToNumber(char *str)
{
	double num=0.0;
	unsigned int dot_num=0;
	bool is_minus=false;
	
	int str_len=strlen(str);
	for(int i=0; i<str_len; ++i)
	{
		if(str[i]=='-') 
		{
			is_minus=true;
		}
		else if(str[i]=='.')
		{
			dot_num=1;
		}
		else if(str[i]>='0' && str[i]<='9')
		{
			num=num*10.0+double(str[i]-48);
			if(dot_num>0) dot_num*=10;
		}
	}

	if(dot_num>1) num/=double(dot_num);
	if(is_minus) num=-num;

	return float(num); 
}

void Tools_CpyCatString(char *dest, ...)
{
	char **cur_param=&dest;
	strcpy(dest,*(++cur_param));
	while(*(++cur_param)!=NULL) strcat(dest, *cur_param);
}

bool TOOLS_FILE::CopyFileToData(char *p_filename)
{
	HANDLE open_file=CreateFileA(p_filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(open_file==INVALID_HANDLE_VALUE) 
		{
			printf("error, read %s\n",p_filename);
			return false; 
		}
		data_len=GetFileSize(open_file,NULL);
		data=new BYTE[data_len+1];
		DWORD readed_bytes;
		ReadFile(open_file, data, data_len, &readed_bytes, NULL);
		data[data_len]=0;
	CloseHandle(open_file);

	cur_pos=(char*)data;

	return true;
}

bool TOOLS_FILE::ReadNextStrings(char **str, int nums)
{
	for(int i=0; i<nums; ++i)
	{
		while( *cur_pos==0 || *cur_pos==10 || *cur_pos==13 || *cur_pos==' ' ) 
		{
			if( (BYTE*)(++cur_pos)>=(BYTE*)(data+data_len) ) 
			{
				str[i]=NULL;
				return false;
			}
		}
		str[i]=cur_pos;

		while( *cur_pos!=0 && *cur_pos!=10 && *cur_pos!=13 && *cur_pos!=' ' ) ++cur_pos;
		*cur_pos=0;
	}

	return true;
}

void TOOLS_FILE::End()
{
	delete [] data;
}