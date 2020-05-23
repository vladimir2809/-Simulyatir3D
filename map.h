#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
using namespace sf;
using namespace std;
const int screenWidth = 600;
const int screenHeigth = 600;

const int quantityBlock = 20;
const int blockSize = 40;

class Blocks
{
	struct Block
	{
		int x, y;
		bool being = false;
	};

public:
	int dx, dy;
	Block arrBlocks[quantityBlock];
	Blocks()
	{
		dx = blockSize, dy = blockSize;

		for (int i = 0; i < quantityBlock; i++)
		{
			arrBlocks[i].being = true;
			arrBlocks[i].x = blockSize*(rand() % (int)(screenWidth / blockSize));
			arrBlocks[i].y = blockSize*(rand() % (int)(screenWidth / blockSize));

		}
	}
	void Draw(RenderWindow &window)
	{
		RectangleShape rectangle(Vector2f(blockSize / 4, blockSize / 4));
		rectangle.setFillColor(Color(128, 128, 128));
		for (int i = 0; i < quantityBlock; i++)
		{
			if (arrBlocks[i].being == true)
			{
				rectangle.setPosition(arrBlocks[i].x / 4, arrBlocks[i].y / 4);
				window.draw(rectangle);


			}
			/*cout << arrBlocks[i].x;
			cout << arrBlocks[i].y;
			cout << "kio";*/
			//int x= 1;
		}
	}
};
