#pragma once

//bool Tools_CompressData(BYTE **dest, int *dest_len, BYTE *source, int source_len);
//bool Tools_DecompressData(BYTE *dest, int *dest_len, BYTE *source, int source_len);
void Tools_IntNumberToString(int num, char *str);
float Tools_StringToNumber(char *str);
void Tools_CpyCatString(char *dest, ...);

class TOOLS_FILE
{
public:
	BYTE *data;
	DWORD data_len;
	char *cur_pos;
	
	bool CopyFileToData(char *p_filename);
	bool ReadNextStrings(char **str, int nums);
	void End();
};
