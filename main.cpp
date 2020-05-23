#include <SFML/Graphics.hpp>
#include "map.h"
#include "bot.h"
#include "player.h"
#include "windows.h"
void SetPos(int x, int y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
int main()
{
	srand(2);
	sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeigth), "Simulyator3D");
	Blocks blocks;
	Bot bot;
	//bot.SetRadius(20);
	bot.SetCoord(1, 1);
	Player player(blocks);
	SetPos(10, 10);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape) == true)
				window.close();
		}
		//Mouse::setPosition(Vector2i(10, 10));// устанновка курсора в пользовательские координаты  

		bot.Services();
		bot.Control();
		player.BotToLine(bot);
		player.Services(window);

		window.clear();
		player.Draw(window);
		blocks.Draw(window);
		bot.Draw(window, 4);
		player.DrawLineBot(window);
		window.display();
	}

}