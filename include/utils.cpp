#include <CL/opencl.hpp>


cl::Platform findCL2Platform() {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  for (auto &p : platforms) {
    std::string version = p.getInfo<CL_PLATFORM_VERSION>();
    if (version.find("OpenCL 2.") != std::string::npos) {
      return p;
    }
  }
  return cl::Platform();
}

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
