#include <cstdio>


char* read_file(char *filename) {
	FILE* file;
	file = fopen(filename, "r");
	fseek(file,0,SEEK_END);
	int file_size = ftell(file);
	fseek(file,0,0);
	char *data = new char[file_size+1];
	fread(data, sizeof(char), file_size, file);
	fclose(file);
	data[file_size] = 0;
	return data;
}
