#include <SFML/Graphics.hpp>


using namespace sf;


struct MyWindow {
private:
	RenderWindow *window;

public:
	MyWindow(unsigned int w, unsigned int h, bool fs = false) {
		window = new RenderWindow(sf::VideoMode(w, h), "window", fs ? Style::Fullscreen : Style::Close);
		//window.setFramerateLimit(60);
	}

	void hideCursor() {window->setMouseCursorVisible(false);}
	void showCursor() {window->setMouseCursorVisible(true);}
	void setCursorPosition(int x, int y) {Mouse::setPosition(Vector2i(x, y), *window);}
	void point(int x, int y, char r, char g, char b) {
		Vertex point(sf::Vector2f((float)x, (float)y), Color(r,g,b));
		window->draw(&point, 1, Points);
	}
	void close() {
		showCursor();
		window->close();
	}
	bool isOpen() {
		return window->isOpen();
	}
	void update() {
		window->display();
	}
	bool pollEvent(Event &event) {
		return window->pollEvent(event);
	}
	void draw(const Drawable &drawable) {
		window->draw(drawable);
	}
	void clear() {
		window->clear();
	}
};
