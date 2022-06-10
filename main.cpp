#include <bitmap.cpp>
#include <octree.cpp>
#include <window.cpp>


Octree *octree;
Bitmap *output;

#define width 720
#define height 480


/*vec4<unsigned char> vox[12] = {
	vec4<unsigned char>(255,255,255,1),vec4<unsigned char>(255,255,255,1),
	vec4<unsigned char>(255,255,255,1),vec4<unsigned char>(255,255,255,1),
	vec4<unsigned char>(255,255,255,1),vec4<unsigned char>(255,255,255,1),
	vec4<unsigned char>(255,255,255,1),vec4<unsigned char>(255,255,255,1),
	vec4<unsigned char>(255,255,255,1),vec4<unsigned char>(255,255,255,1),
	vec4<unsigned char>(255,255,255,1),vec4<unsigned char>(255,255,255,1),
};*/


int main() {
	srand(time(NULL));

	octree = new Octree(4);
	octree->get(0)->setFinal(false);
	octree->get(2)->setFinal(false);
	octree->get(3)->setFinal(false);
	octree->get(6)->setFinal(false);

	octree->get(0)->get(0)->visible = true;
	octree->get(0)->get(0)->color = vec3<unsigned char>(255,255,255);
	octree->get(0)->get(2)->visible = true;
	octree->get(0)->get(2)->color = vec3<unsigned char>(255,255,255);
	octree->get(0)->get(3)->visible = true;
	octree->get(0)->get(3)->color = vec3<unsigned char>(255,255,255);

	octree->get(2)->get(1)->visible = true;
	octree->get(2)->get(1)->color = vec3<unsigned char>(255,255,255);
	octree->get(2)->get(5)->visible = true;
	octree->get(2)->get(5)->color = vec3<unsigned char>(255,255,255);

	octree->get(3)->get(0)->visible = true;
	octree->get(3)->get(0)->color = vec3<unsigned char>(255,255,255);
	octree->get(3)->get(2)->visible = true;
	octree->get(3)->get(2)->color = vec3<unsigned char>(255,255,255);
	octree->get(3)->get(3)->visible = true;
	octree->get(3)->get(3)->color = vec3<unsigned char>(255,255,255);

	octree->get(6)->get(1)->visible = true;
	octree->get(6)->get(1)->color = vec3<unsigned char>(255,255,255);

	octree->get(4)->visible = true;
	octree->get(4)->color = vec3<unsigned char>(255,255,255);

	octree->getVisible();

	MyWindow window(width, height);
	window.hideCursor();
	window.setCursorPosition(width / 2, height / 2);

	Clock clock;
	float lastTime = 0;

	sf::Font font;
	if (!font.loadFromFile("/usr/share/fonts/adobe-source-code-pro/SourceCodePro-Medium.otf")) return -1;
	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(12);
	text.setFillColor(sf::Color::Red);

	//output = new Bitmap(width, height);

	vec3f ro(0,0,-10);
	vec3f vel;
	vec3f rot(0);
	float cam_speed = 1;
	float cam_rot = 0.01;

	while (window.isOpen()) {
		window.clear();

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

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				float u = (float)x / width  * 2. - 1.;
				float v = (float)y / height * 2. - 1.;
				vec3f rd = normalize(vec3f(u * ((float)width / height),v,1)) * rot_mat;

				vec4<unsigned char> col = octree->trayce(ro, rd, 3);

				if (col.w == 1) {
					window.point(x, y, col.x,col.y,col.z);
				}
			}
		}

		float currentTime = clock.getElapsedTime().asSeconds();
		float fps = 1.f / (currentTime - lastTime);
		lastTime = currentTime;
		text.setString(std::to_string(fps));
		window.draw(text);

		window.update();

		window.setCursorPosition(width / 2, height / 2);
	}

	//output->save((char*)"output.bmp");
}
