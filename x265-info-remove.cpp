#include <string.h>
#include <cstdint>
#include <cstdio>

#define BUILD 250117

int main(int argv, char** argc)
{
	FILE* ofile;
	FILE* ifile;
	errno_t err;
	int allflag = 0;
	int y, ptr;
	uint64_t read_counter = 0;
	uint64_t write_counter = 0;
	int num_nal = 0;
	int buffer[6] = { 0,0,0,0,0,0 }; //circular buffer
	bool pen = true;

	///// HELP
	if (((argv < 3) || (argv > 4)) || ((argv == 4) && (strcmp("all", argc[1]) != 0)))
	{
		printf("x265-info remove - Build %d\nUSAGE:\n\nA) x265-info-remove.exe inputFile.hevc outputFile.hevc\nB) x265-info-remove.exe all inputFile.hevc outputFile.hevc\n\n",BUILD);
		printf("The script looks for each occurence of the hex string 00 00 01 4E 01 05 in inputFile and removes it, together with following bytes up to next 00 00 01\n\n");
		printf("If the option 'all' is not specified, the first occurrence is kept\n\n");
		printf("The script is useful to remove unwanted info user-defined nal-units (encoding settings) from raw x265 hevc stream\n\n");
		printf("WARNING: use only on raw .hevc stream ( NOT .mkv NOT .mp4 )\n");
		return 0;
	}

	///// OPEN FILES
	if (argv == 4) allflag = 1;
	err = fopen_s(&ifile, argc[1 + allflag], "rb");
	if ((ifile == NULL) || (err != 0))
	{
		printf("Error while opening inputFile\n");
		return 1;
	}
	err = fopen_s(&ofile, argc[2 + allflag], "wb");
	if ((ofile == NULL) || (err != 0))
	{
		printf("Error while opening outputFile\n");
		return 1;
	}

	/////FILL INITIAL BUFFER
	for (ptr = 0; ptr < 6; ptr++)
	{
		buffer[ptr] = fgetc(ifile);
		read_counter++;
		if (buffer[ptr] == EOF) break;	
	}
	if (buffer[ptr] == EOF) //inputFile < 6 bytes
	{
		for (y = 0; y < ptr; y++)
		{
			fputc(buffer[y], ofile);
			write_counter++;
		}
	}
	else
	{
		ptr = 0;
		while (buffer[(ptr + 5) % 6] != EOF) ///// MAIN LOOP
		{
			if ((buffer[ptr] == 0) && (buffer[(ptr + 1) % 6] == 0) && (buffer[(ptr + 2) % 6] == 1))
			{
				pen = true;
				if ((buffer[(ptr + 3) % 6] == 78) && (buffer[(ptr + 4) % 6] == 1) && (buffer[(ptr + 5) % 6] == 5))
				{
					if ((num_nal > 0) || (allflag == 1))
						pen = false;
					num_nal++;
				}
			}
			if (pen)
			{
				fputc(buffer[ptr], ofile);
				write_counter++;
			}
			buffer[ptr] = fgetc(ifile);
			ptr = (ptr + 1) % 6;
			read_counter++;
		}
		//FLUSH
		if (pen)
		{
			write_counter += 5;
			fputc(buffer[ptr], ofile);
			fputc(buffer[(ptr + 1) % 6], ofile);
			fputc(buffer[(ptr + 2) % 6], ofile);
			fputc(buffer[(ptr + 3) % 6], ofile);
			fputc(buffer[(ptr + 4) % 6], ofile);
		}
	}
	read_counter--;//remove EOF counter

	///// WRITE INFO AND CLOSE
	fclose(ifile);
	fclose(ofile);

	printf("User-data nal unit found: %d\n", num_nal);
	printf("User-data nal unit removed: %d\n", ((allflag==1)||(num_nal==0)) ? num_nal : num_nal-1 );
	printf("Bytes read: %ju\n", read_counter);
	printf("Bytes written: %ju\n", write_counter);
	printf("Bytes removed: %ju\n", read_counter - write_counter);
	return 0;
}


