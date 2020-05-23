#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;
class Bot
{
public:
	float x, y, speed;
	int size;
	CircleShape shape;
	Bot()
	{

		speed = 5;
		//size = blockSize / 2;
		size = 5;
		shape.setRadius(size);
		// задаём фигуре зелёный цвет
		shape.setFillColor(sf::Color(100, 250, 50));
	}
	void SetCoord(int xx, int yy)
	{
		x = xx;
		y = yy;
		shape.setPosition(x / 4, y / 4);
	}
	void Control()
	{
		if (Keyboard::isKeyPressed(Keyboard::Left)) x -= speed;
		if (Keyboard::isKeyPressed(Keyboard::Up)) y -= speed;
		if (Keyboard::isKeyPressed(Keyboard::Right)) x += speed;
		if (Keyboard::isKeyPressed(Keyboard::Down)) y += speed;
		shape.setPosition(x / 4, y / 4);
	}
	void Services()
	{
		Control();
	}
	void Draw(RenderWindow &window, int divider = 1)
	{
		shape.setRadius(size / divider);

		window.draw(shape);
	}
};