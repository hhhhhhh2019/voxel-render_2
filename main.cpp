#include <utils.cpp>
#include <octree.cpp>
#include <SFML/Graphics.hpp>


using namespace sf;


#define width  720
#define height 480


Node *octree;
vec3f output[width * height];

typedef Glsl::Vec4 vec4g;

vec4g model[64] = {
	vec4g(0,0,0,0),vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(0,0,0,0),
	vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(1,1,1,1),

	vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(1,1,1,1),
	vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(0,0,0,0),

	vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),
	vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),

	vec4g(0,0,0,0),vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(1,1,1,1),
	vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(1,1,1,1),

	vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(0,0,0,0),
	vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(0,0,0,0),

	vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(1,1,1,1),vec4g(0,0,0,0),
	vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),

	vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(0,0,0,0),
	vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(1,1,1,1),vec4g(1,1,1,1),

	vec4g(1,1,1,1),vec4g(0,0,0,0),vec4g(1,1,1,1),vec4g(1,1,1,1),
	vec4g(0,0,0,0),vec4g(0,0,0,0),vec4g(1,1,1,1),vec4g(1,1,1,1)
};

int main() {
	srand(time(NULL));


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
	shader.setUniform("size", 3);
	shader.setUniformArray("model", model, 64);


	vec3f ro(0,0,-100);
	vec3f rot(0,0,0);
	vec3f vel;
	float cam_speed = 1;
	float cam_rot = 0.01;

	int _w = width;
	int _h = height;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}
			else if (event.type == Event::MouseMoved) {
				float mx = event.mouseMove.x - width / 2;
				float my = event.mouseMove.y - height / 2;

				rot.x += my * cam_rot;
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


		shader.setUniform("ro", Vector3f(ro.x,ro.y,ro.z));
		shader.setUniform("rot", Glsl::Mat3(new float[]{
			rot_mat.a,rot_mat.b,rot_mat.c,
			rot_mat.d,rot_mat.e,rot_mat.f,
			rot_mat.g,rot_mat.h,rot_mat.i
		}));


		RenderTexture image;
		image.create(width, height);

		Sprite drawable(image.getTexture());

		image.draw(drawable, &shader);
		image.display();

		window.draw(drawable);


		float currentTime = clock.getElapsedTime().asSeconds();
		float fps = 1.f / (currentTime - lastTime);
		lastTime = currentTime;

		text.setString(std::to_string(fps));

		window.draw(text);

		window.display();


		Mouse::setPosition(Vector2i(width / 2, height / 2), window);
	}
}
