#include <utils.cpp>
#include <octree.cpp>
#include <SFML/Graphics.hpp>


using namespace sf;


#define width  720
#define height 480


Node *octree;
vec3f output[width * height];

typedef Glsl::Vec4 vec4g;

vec4g model[512] = {
	/*vec4g(0,0,0,0),   vec4g(.4,.4,.4,1),vec4g(0,0,0,0),   vec4g(0,0,0,0),
	vec4g(.4,.4,.4,1),vec4g(.4,.4,.4,1),vec4g(0,0,0,0),   vec4g(.1,.9,.1,-2),

	vec4g(.4,.4,.4,1),vec4g(.4,.4,.4,1),vec4g(0,0,0,0),   vec4g(.4,.5,.9,1),
	vec4g(.4,.4,.4,1),vec4g(.4,.4,.4,1),vec4g(0,0,0,0),   vec4g(0,0,0,0),

	vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),
	vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),

	vec4g(0,0,0,0),   vec4g(.4,.5,.9,1),vec4g(0,0,0,0),   vec4g(.4,.5,.9,1),
	vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(.9,.3,.3,1),

	vec4g(.4,.4,.4,1),vec4g(.4,.4,.4,1),vec4g(0,0,0,0),   vec4g(0,0,0,0),
	vec4g(.4,.4,.4,1),vec4g(.4,.4,.4,1),vec4g(.4,.5,.9,1),vec4g(0,0,0,0),

	vec4g(.4,.4,.4,1),vec4g(.4,.4,.4,1),vec4g(.4,.5,.9,1),vec4g(0,0,0,0),
	vec4g(.4,.4,.4,1),vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),

	vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(0,0,0,0),
	vec4g(.4,.5,.9,1),vec4g(0,0,0,0),   vec4g(.4,.5,.9,1),vec4g(.9,.3,.3,1),

	vec4g(.4,.5,.9,1),vec4g(0,0,0,0),   vec4g(.4,.5,.9,1),vec4g(.9,.3,.3,1),
	vec4g(0,0,0,0),   vec4g(0,0,0,0),   vec4g(.9,.3,.3,1),vec4g(.9,.3,.3,1)*/
};

int main() {
	srand(time(NULL));

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 8; k++) {
				int id = i * 8 * 8 + j * 8 + k;

				model[id] = vec4g(0,0,0,0);
				//if (rand() > RAND_MAX / 2) model[id].w = -2;

				int x = i % 2 * 4 + j % 2 * 2 + k % 2;
				int y = (i % 4 / 2) * 4 + (j % 4 / 2) * 2 + k % 4 / 2;
				int z = i / 4 * 4 + j / 4 * 2 + k / 4;

				//printf("%i %i\n", x, id);

				float r = 1;
				float g = 1;
				float b = 1;

				if (x == 0 && z == 0) model[id] = vec4g(r,g,b,-2);
				if (x == 7 && z == 0) model[id] = vec4g(r,g,b,-2);
				if (x == 0 && z == 7) model[id] = vec4g(r,g,b,-2);
				if (x == 7 && z == 7) model[id] = vec4g(r,g,b,-2);
				if (y == 0 && z == 0) model[id] = vec4g(r,g,b,-2);
				if (y == 7 && z == 0) model[id] = vec4g(r,g,b,-2);
				if (y == 0 && z == 7) model[id] = vec4g(r,g,b,-2);
				if (y == 7 && z == 7) model[id] = vec4g(r,g,b,-2);
				if (x == 0 && y == 0) model[id] = vec4g(r,g,b,-2);
				if (x == 7 && y == 0) model[id] = vec4g(r,g,b,-2);
				if (x == 0 && y == 7) model[id] = vec4g(r,g,b,-2);
				if (x == 7 && y == 7) model[id] = vec4g(r,g,b,-2);

				if (x == 3 && y == 3 && z == 3) model[id] = vec4g(r,g,b,1);
				if (x == 4 && y == 3 && z == 3) model[id] = vec4g(r,g,b,1);
				if (x == 3 && y == 4 && z == 3) model[id] = vec4g(r,g,b,1);
				if (x == 4 && y == 4 && z == 3) model[id] = vec4g(r,g,b,1);
				if (x == 3 && y == 3 && z == 4) model[id] = vec4g(r,g,b,1);
				if (x == 4 && y == 3 && z == 4) model[id] = vec4g(r,g,b,1);
				if (x == 3 && y == 4 && z == 4) model[id] = vec4g(r,g,b,1);
				if (x == 4 && y == 4 && z == 4) model[id] = vec4g(r,g,b,1);
			}
		}
	}

	/*for (int i = 0; i < 512; i++) {
		model[i] = vec4g(1,1,1,-2);
	}*/


	//octree = new Node(3, 0, model);

	//octree->getVisible();


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


	Shader shader;

	if (!shader.loadFromFile("shader.frag", Shader::Fragment)) {
		printf("can't load shader");
		return 1;
	}

	shader.setUniform("resolution", Vector2f(width,height));
	shader.setUniform("model_size", 4);
	shader.setUniformArray("model", model, 512);

	//shader.setUniform("u_seed1", Vector2f(rand(), rand()));//*999));
	//shader.setUniform("u_seed2", Vector2f(rand(), rand()));//*999));


	vec3f ro(0,0,-128);//(-10,5,-10);
	vec3f rot(0,0,0);//(3.14 / 8.7,-3.14 / 4,0);
	vec3f vel;
	float cam_speed = 1;
	float cam_rot = 0.01;

	/*int _w = width;
	int _h = height;*/

	RenderTexture image;
	image.create(width, height);

	Sprite drawable(image.getTexture());

	bool useBackBuffer = false;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			else if (event.type == Event::MouseMoved) {
				float mx = event.mouseMove.x - width / 2;
				float my = event.mouseMove.y - height / 2;

				if (length(vec2f(mx,my)) > 1) {
					rot.x += my * cam_rot;
					rot.y -= mx * cam_rot;

					useBackBuffer = false;
				}
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

		if (length(vel) != 0) useBackBuffer = false;

		mat3 rot_mat = mat3(
			cos(rot.z), -sin(rot.z), 0,
			sin(rot.z), cos(rot.z),  0,
			0,          0,           1
		) * mat3(
			cos(rot.y), 0, -sin(rot.y),
			0,          1, 0,
			sin(rot.y), 0, cos(rot.y)
		) * mat3(
			1, 0,          0,
			0, cos(rot.x), -sin(rot.x),
			0, sin(rot.x), cos(rot.x)
		);

		ro = ro + vel * rot_mat;

		window.clear();


		shader.setUniform("cam_pos", Vector3f(ro.x,ro.y,ro.z));
		shader.setUniform("cam_rot", Glsl::Mat3(new float[]{
			rot_mat.a,rot_mat.b,rot_mat.c,
			rot_mat.d,rot_mat.e,rot_mat.f,
			rot_mat.g,rot_mat.h,rot_mat.i
		}));
		shader.setUniform("u_seed1", Vector2f((float)rand() / RAND_MAX - 0.5, ((float)rand() / RAND_MAX - 0.5) * 999));
		shader.setUniform("u_seed2", Vector2f((float)rand() / RAND_MAX - 0.5, ((float)rand() / RAND_MAX - 0.5) * 999));
		shader.setUniform("backBuffer", image.getTexture());
		shader.setUniform("useBackBuffer", useBackBuffer);

		image.draw(drawable, &shader);
		image.display();

		window.draw(drawable);

		useBackBuffer = true;


		float currentTime = clock.getElapsedTime().asSeconds();
		float fps = 1.f / (currentTime - lastTime);
		lastTime = currentTime;

		text.setString(std::to_string(fps));

		window.draw(text);

		window.display();


		Mouse::setPosition(Vector2i(width / 2, height / 2), window);
	}
}
