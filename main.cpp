#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <utils.cpp>
#include <octree.cpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define width 720
#define height 480


Octree *octree;
char* kernel_prog;
vec3f output[width * height];



void processInput(GLFWwindow*);
void framebuffer_size_callback(GLFWwindow*, int, int);


char* frag_shader_data;
char* vert_shader_data;
unsigned int frag_shader;
unsigned int vert_shader;
unsigned int shader_prog;

int initShaders() {
	frag_shader_data = read_file((char*)"shader.frag");
	vert_shader_data = read_file((char*)"shader.vert");

	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_data, NULL);
	glCompileShader(vert_shader);

	int success;
	char infoLog[512];
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
		printf(infoLog);
		printf("\n");
		return 0;
	}

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_data, NULL);
	glCompileShader(frag_shader);

	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
		printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
		printf(infoLog);
		printf("\n");
		return 0;
	}

	shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vert_shader);
	glAttachShader(shader_prog, frag_shader);
	glLinkProgram(shader_prog);
	glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_prog, 512, NULL, infoLog);
		printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
		printf(infoLog);
		printf("\n");
		return 0;
	}
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return 1;
}


int main() {
	kernel_prog = read_file((char*)"render.cl");

	srand(time(NULL));


	octree = new Octree(4);
	octree->get(0)->setFinal(false);
	octree->get(2)->setFinal(false);
	octree->get(3)->setFinal(false);
	octree->get(6)->setFinal(false);

	octree->get(0)->get(0)->visible = true;
	octree->get(0)->get(0)->color = vec3f(255,255,255);
	octree->get(0)->get(2)->visible = true;
	octree->get(0)->get(2)->color = vec3f(255,255,255);
	octree->get(0)->get(3)->visible = true;
	octree->get(0)->get(3)->color = vec3f(255,255,255);

	octree->get(2)->get(1)->visible = true;
	octree->get(2)->get(1)->color = vec3f(255,255,255);
	octree->get(2)->get(5)->visible = true;
	octree->get(2)->get(5)->color = vec3f(255,255,255);

	octree->get(3)->get(0)->visible = true;
	octree->get(3)->get(0)->color = vec3f(255,255,255);
	octree->get(3)->get(2)->visible = true;
	octree->get(3)->get(2)->color = vec3f(255,255,255);
	octree->get(3)->get(3)->visible = true;
	octree->get(3)->get(3)->color = vec3f(255,255,255);

	octree->get(6)->get(1)->visible = true;
	octree->get(6)->get(1)->color = vec3f(255,255,255);

	octree->get(4)->visible = true;
	octree->get(4)->color = vec3f(255,255,255);

	octree->getVisible();


	printf("Init OpenCL\n");

	cl::Platform platform = findCL2Platform();

	if (platform() == 0) {
		printf("No OpenCL 2 platform found.\n");
		return -1;
	}

	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	if (devices.size() == 0) {
		printf("No OpenCL 2 GPU found.\n");
		return -1;
	}

	cl::Device device = devices[0];


	printf("Device info:\n\t");
	printf("CL_DEVICE_NAME: %s", device.getInfo<CL_DEVICE_NAME>().c_str());
	printf("\n\t");
	printf("CL_DEVICE_VENDOR: %s", device.getInfo<CL_DEVICE_VENDOR>().c_str());
	printf("\n\t");
	printf("CL_DEVICE_VERSION: %s", device.getInfo<CL_DEVICE_VERSION>().c_str());
	printf("\n\t");
	printf("CL_DEVICE_ADDRESS_BITS: %i", device.getInfo<CL_DEVICE_ADDRESS_BITS>());
	printf("\n\t");
	printf("CL_DEVICE_AVAILABLE: %i", device.getInfo<CL_DEVICE_AVAILABLE>());
	printf("\n\t");
	printf("CL_DEVICE_COMPILER_AVAILABLE: %i", device.getInfo<CL_DEVICE_COMPILER_AVAILABLE>());
	printf("\n\t");
	printf("CL_DEVICE_TYPE: %i", device.getInfo<CL_DEVICE_TYPE>());

	printf("\n");

	cl_int err;

	cl::Context context(device);
	cl::CommandQueue queue(context, device, 0, &err);

	if (err != CL_SUCCESS) {
		printf("Unable to create OpenCL command queue!\nError: %i", err);
		return -1;
	}

	cl::Program program(kernel_prog);

	try {
    program.build("-cl-std=CL2.0");
  }
  catch (cl::BuildError e) {
    printf("Error building program render.cl: ");
		printf((char*)e.what());
		printf("\n");
    for (auto &pair : e.getBuildLog()) {
      printf(pair.second.c_str());
			printf("\n");
    }

    return -1;
  }

	cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> kernel(program, "kmain", &err);

	if (err != CL_SUCCESS) {
		printf("Unable to create kernel!\nError: %i", err);
		return -1;
	}


	printf("Init glfw\n");

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "window", NULL, NULL);

	if (window == NULL) {
		printf("Failed to create window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	if (!initShaders()) {
		printf("Failed to initialize shaders\n");
		return -1;
	}

	vec3f ro(0,0,0);
	vec3f rot(0,0,0);

	cl::Buffer roBuf(&ro, &ro + sizeof(float) * 3, true);
	cl::Buffer rotBuf(&rot, &rot + sizeof(float) * 3, true);
	cl::Buffer outBuf(std::begin(output), std::end(output), true);

	unsigned char *pixels[width * height * 3];

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glBindTexture(GL_TEXTURE_2D, 1);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);

		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1,1,1,-1,1,100);
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex3f(-1,-1,-1);
		glTexCoord2f(0,0);
		glVertex3f(-1,1,-1);
		glTexCoord2f(1,0);
		glVertex3f(1,1,-1);
		glTexCoord2f(1,1);
		glVertex3f(1,-1,-1);
		glEnd();*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}


void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int _width, int _height) {
	printf("width: %i, height: %i\n", _width, _height);
	glViewport(0, 0, _width, _height);
}
