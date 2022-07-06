#include <utils.cpp>
#include <octree.cpp>
#include <SFML/Graphics.hpp>


using namespace sf;


#define width  360
#define height 240


Node *octree;
char *kernel_prog;
vec3f output[width * height];

char model[64] = {
	1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0,
	1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0,
	1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0,
	1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,0
};

int main() {
	kernel_prog = read_file((char*)"render.cl");

	srand(time(NULL));


	octree = new Node(3, 0, model);

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

	cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> kernel(program, "kmain", &err);

	if (err != CL_SUCCESS) {
		printf("Unable to create kernel!\nError: %i", err);
		return -1;
	}


	Font font;
	if (!font.loadFromFile("/usr/share/fonts/adobe-source-code-pro/SourceCodePro-Medium.otf")) return -1;
	Text text;
	text.setFont(font);
	text.setCharacterSize(12);
	text.setFillColor(Color::Red);

	Clock clock;
	float lastTime = 0;

	RenderWindow window(VideoMode(width, height), "window");//, Style::Fullscreen);
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);

	Mouse::setPosition(Vector2i(width / 2, height / 2), window);

	vec3f ro(0,0,-10);
	vec3f rot(0,0,0);
	vec3f vel;
	float cam_speed = 0.2;
	float cam_rot = 0.01;

	int _w = width;
	int _h = height;

	cl::Buffer roBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(float) * 3);
	cl::Buffer rotBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(float) * 9);
	cl::Buffer outBuf(std::begin(output), std::end(output), true);
	cl::Buffer widthBuf(&_w, &_w + sizeof(int), true);
	cl::Buffer heightBuf(&_h, &_h + sizeof(int), true);
	cl::Buffer objBuf(context, CL_MEM_READ_ONLY | CL_MEM_HOST_WRITE_ONLY, sizeof(char) * 64);
	cl::Buffer cache(context, CL_MEM_READ_WRITE, sizeof(char) * 30 * 16);

	unsigned char pixels[width * height * 4];

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			else if (event.type == Event::MouseMoved) {
				float mx = event.mouseMove.x - width / 2;
				float my = event.mouseMove.y - height / 2;

				rot.x -= my * cam_rot;
				rot.y -= mx * cam_rot;
			}
			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Escape) {
					window.setMouseCursorVisible(true);
					window.close();
				}
				else if (event.key.code == Keyboard::W) vel.z = cam_speed;
				else if (event.key.code == Keyboard::A) vel.x = -cam_speed;
				else if (event.key.code == Keyboard::S) vel.z = -cam_speed;
				else if (event.key.code == Keyboard::D) vel.x = cam_speed;
				else if (event.key.code == Keyboard::Q) vel.y = -cam_speed;
				else if (event.key.code == Keyboard::E) vel.y = cam_speed;
			}
			else if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::W) vel.z = 0;
				else if (event.key.code == Keyboard::A) vel.x = 0;
				else if (event.key.code == Keyboard::S) vel.z = 0;
				else if (event.key.code == Keyboard::D) vel.x = 0;
				else if (event.key.code == Keyboard::Q) vel.y = 0;
				else if (event.key.code == Keyboard::E) vel.y = 0;
			}
		}

		mat3 rot_mat = mat3(
			cos(rot.z), -sin(rot.z), 0,
			sin(rot.z), cos(rot.z), 0,
			0,       0, 1
		) * mat3(
			cos(rot.y), 0, -sin(rot.y),
			0,      1, 0,
			sin(rot.y), 0, cos(rot.y)
		) * mat3(
			1,       0, 0,
			0, cos(rot.x), -sin(rot.x),
			0, sin(rot.x), cos(rot.x)
		);

		ro = ro + vel * rot_mat;

		window.clear();

		clEnqueueWriteBuffer(queue(), roBuf(),  true, 0, sizeof(float) * 3, &ro,  0, NULL, NULL);
		clEnqueueWriteBuffer(queue(), rotBuf(), true, 0, sizeof(float) * 9, &rot_mat, 0, NULL, NULL);
		clEnqueueWriteBuffer(queue(), objBuf(), true, 0, sizeof(char) * 64, &model, 0, NULL, NULL);

		kernel(cl::EnqueueArgs(cl::NDRange(width * height)),
			roBuf, rotBuf, outBuf, widthBuf, heightBuf, objBuf, cache);

		cl::copy(outBuf, std::begin(output), std::end(output));

		for (int i = 0; i < width * height; i++) {
			pixels[i * 4 + 0] = output[i].x * 255;
			pixels[i * 4 + 1] = output[i].y * 255;
			pixels[i * 4 + 2] = output[i].z * 255;
			pixels[i * 4 + 3] = 255;
		}

		Image img;
		img.create(width, height, pixels);

		Texture texture;
		texture.loadFromImage(img);
		Sprite sprite;
		sprite.setTexture(texture, true);


		float currentTime = clock.getElapsedTime().asSeconds();
		float fps = 1.f / (currentTime - lastTime);
		lastTime = currentTime;

		text.setString(std::to_string(fps));

		window.draw(text);

		window.display();


		Mouse::setPosition(Vector2i(width / 2, height / 2), window);
	}
}
