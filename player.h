#include <SFML/Graphics.hpp>
#include <math.h>
//#include "map.h"
using namespace sf;
using namespace std;
//const float FieldOfView = 2;
//const float StepOfField = 0.002;
const float pi = 3.1415926;
const int amountLines = 1001;
bool mouseCapture = true;// ������ ���� �����������
void DrawLine(int x, int y, int x1, int y1, Color color, RenderWindow &window)// ��������� ��������� �����
{
	//������� ������ �����, �� ������� ����� ���������� �����:
	sf::VertexArray lines(sf::Lines, 16 /*���������� �����*/);

	//����� ��� ������ �� ����� ������ ���� �������:
	lines[0].position = sf::Vector2f(x, y);
	lines[1].position = sf::Vector2f(x1, y1);
	//� �.�.

	//����� ��� ������ ����� ��������� ����(��� ����� ��������� ���������):
	lines[0].color = color;
	lines[1].color = color;
	//� �.�.

	//� � ����� ������� ��� �� �����:
	window.draw(lines);
}
class Player
{
public:

	Vector2f point;
	enum Type// ���� ��������
	{
		EMPTY, WALL, BOT, BOTCENTER
	};
	struct DataLine// ������ �� �������� ����
	{
		Vector2f begin;
		Vector2f end;
	};
	struct Data// ������ � ������, ������� �����������������
	{
		float dist;// ���������
		Vector2f acrossPoint;// ����� �����������
		Color color;
		Type type;
	};
	DataLine dataLine[quantityBlock * 4];// ������ ������ � ������
	DataLine dataLineBot[4];
	Data data[amountLines];// ������ ����� ���������
	Blocks blocks;
	Bot bot;
	float dir;// ����������
	float FieldOfView;// ���� ������� ��������� � ���
	float StepOfField;// ���� ����� ������
	float WalkSpeed;// �������� ������
	float ViewSpeed;//�������� ������ �����
	float MaxDistance;// ������������ ��������� ���������

	void BlockToLines()// ������� ����������� � ����������� ����� ������ � ����� ����
	{
		for (int i = 0; i < quantityBlock; i++)
		{
			Vector2f A = Vector2f(blocks.arrBlocks[i].x, blocks.arrBlocks[i].y);
			Vector2f B = Vector2f(blocks.arrBlocks[i].x + blocks.dx, blocks.arrBlocks[i].y);
			Vector2f C = Vector2f(blocks.arrBlocks[i].x + blocks.dx, blocks.arrBlocks[i].y + blocks.dy);
			Vector2f D = Vector2f(blocks.arrBlocks[i].x, blocks.arrBlocks[i].y + blocks.dy);

			dataLine[i * 4 + 0].begin = A;
			dataLine[i * 4 + 0].end = B;

			dataLine[i * 4 + 1].begin = B;
			dataLine[i * 4 + 1].end = C;

			dataLine[i * 4 + 2].begin = C;
			dataLine[i * 4 + 2].end = D;

			dataLine[i * 4 + 3].begin = D;
			dataLine[i * 4 + 3].end = A;
		}
	}
	void BotToLine(Bot &bot1)
	{
		bot = bot1;
		dataLineBot[0].begin = Vector2f(bot.x, bot.y);
		dataLineBot[0].end = Vector2f(bot.x + bot.size*2, bot.y );

		dataLineBot[1].begin = Vector2f(bot.x + bot.size * 2, bot.y);
		dataLineBot[1].end = Vector2f(bot.x + bot.size * 2, bot.y + bot.size*2);

		dataLineBot[2].begin = Vector2f(bot.x , bot.y+bot.size*2);
		dataLineBot[2].end = Vector2f(bot.x + bot.size * 2, bot.y+bot.size*2);

		dataLineBot[3].begin = Vector2f(bot.x, bot.y);
		dataLineBot[3].end = Vector2f(bot.x , bot.y + bot.size * 2);
	}
	Player(Blocks &blos)
	{
		blocks = blos;
		point.x = screenWidth / 2;
		point.y = screenHeigth / 2;
		dir = 2.5;
		FieldOfView = 1;
		StepOfField = 0.001;
		WalkSpeed = 0.04;
		ViewSpeed = 0.002f;
		MaxDistance = 250;
		BlockToLines();
	}

	float distance(float x, float y, float x1, float y1)
	{
		float dx = abs(x1 - x),
			dy = abs(y1 - y);
		return sqrt(dx*dx + dy*dy);
	}
	float VectMult(float ax, float ay, float bx, float by)
	{
		return (ax * by) - (ay * bx);
	}

	bool IsCrossing(float a1x, float a1y, float a2x, float a2y, float b1x, float b1y, float b2x, float b2y)
	{
		float v1 = VectMult(b2x - b1x, b2y - b1y, a1x - b1x, a1y - b1y);
		float v2 = VectMult(b2x - b1x, b2y - b1y, a2x - b1x, a2y - b1y);
		float v3 = VectMult(a2x - a1x, a2y - a1y, b1x - a1x, b1y - a1y);
		float v4 = VectMult(a2x - a1x, a2y - a1y, b2x - a1x, b2y - a1y);
		return (v1 * v2) <= 0 && (v3 * v4) <= 0;
	}

	Vector2f CrossingPoint(float a1, float b1, float c1, float a2, float b2, float c2)
	{
		float d = a1 * b2 - b1 * a2;
		float dx = -c1 * b2 + b1 * c2;
		float dy = -a1 * c2 + c1 * a2;
		return Vector2f(dx / d, dy / d);
	}

	Vector2f GetCrossVector(Vector2f aP1, Vector2f aP2, Vector2f bP1, Vector2f bP2)
	{
		float a1, b1, c1, a2, b2, c2;
		a1 = aP2.y - aP1.y;
		b1 = aP1.x - aP2.x;
		c1 = -aP1.x * (aP2.y - aP1.y) + aP1.y * (aP2.x - aP1.x);
		a2 = bP2.y - bP1.y;
		b2 = bP1.x - bP2.x;
		c2 = -bP1.x * (bP2.y - bP1.y) + bP1.y * (bP2.x - bP1.x);
		return CrossingPoint(a1, b1, c1, a2, b2, c2);
	}

	void Control(RenderWindow &window)// ������� ���������� � ����������
	{
		float speed = 1.5;
		if (Keyboard::isKeyPressed(Keyboard::W))// ���� ������ 
		{
			point.x += sin(dir) * speed;
			point.y += cos(dir) * speed;
		}
		if (Keyboard::isKeyPressed(Keyboard::S))// ���� ����� 
		{
			point.x -= sin(dir) * speed;
			point.y -= cos(dir) * speed;

		}
		if (Keyboard::isKeyPressed(Keyboard::A))// ������� ������ ������� 
		{
			point.x -= sin(dir - 1.57) * speed;
			point.y -= cos(dir - 1.57) * speed;
		}
		if (Keyboard::isKeyPressed(Keyboard::D))// ������� �� �������
		{
			point.x -= sin(dir + 1.57) * speed;
			point.y -= cos(dir + 1.57) * speed;
		}
		static bool flagBeginProg = false;
		if (flagBeginProg == false)
		{
			flagBeginProg = true;
			window.setMouseCursorVisible(false);
		}
		if (Keyboard::isKeyPressed(Keyboard::M))// ������� �� �������
		{
			mouseCapture = !mouseCapture;
			//window.setMouseCursorVisible() // �������� ������
			window.setMouseCursorVisible(!mouseCapture);


			while (Keyboard::isKeyPressed(Keyboard::M))
			{

			}
		}

		if (mouseCapture == true)
		{
			static Vector2i oldMouse = Mouse::getPosition(window);
			Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
			Vector2i winPos = window.getPosition();
			if (oldMouse.x > mousePos.x)
			{
				dir += (oldMouse.x - mousePos.x) / (float)500;
			}
			else if (oldMouse.x < mousePos.x)
			{
				dir -= (mousePos.x - oldMouse.x) / (float)500;
			}
			Mouse::setPosition(Vector2i(winPos.x + screenWidth / 2, winPos.y + screenHeigth / 2));// ���������� ������� � ���������������� ����������  
			oldMouse = Mouse::getPosition(window);
		}


	}
	void Services(RenderWindow &window)// ������� ������� ���������� � ���� ���� ��������� ����� ����
	{
		Control(window);
		bool reyBotCenter = false;
		float st = -FieldOfView / 2;// ����������� ��������� ���� ������ ��� ������ �����
		for (int i = 0; i < amountLines; i++)
		{
			// ����������� ����� ����������� � ����� ������� ������� ����� ����
			data[i].type = EMPTY;
			data[i].acrossPoint = Vector2f(
				(float)(MaxDistance   * sin(dir + st) + point.x),
				(float)(MaxDistance * cos(dir + st) + point.y));
		
			for (int j = 0; j < quantityBlock * 4; j++)
			{

				if (data[i].type == EMPTY)
				{
					// ��������� ���� ����������� �� ������
					if (IsCrossing(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y,
						dataLine[j].begin.x, dataLine[j].begin.y, dataLine[j].end.x, dataLine[j].end.y))
					{
						// �������������� ����� ������������
						data[i].acrossPoint = GetCrossVector(point, data[i].acrossPoint, dataLine[j].begin, dataLine[j].end);


						//���� ����� ����������� ����� � ����� ����� �����
						if (distance(data[i].acrossPoint.x, data[i].acrossPoint.y, dataLine[j].begin.x, dataLine[j].begin.y) < 0.5 ||
							distance(data[i].acrossPoint.x, data[i].acrossPoint.y, dataLine[j].end.x, dataLine[j].end.y) < 0.5)
							data[i].color = Color::White;
						else
							data[i].color = Color::Green;

					}
					//data[i].type = WALL;
					// ����������� ��������� �� ������ �� ����� ����������� �� ������
					data[i].dist = distance(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y);
					// �������� �� ������� ���� ����� ����������� �� ������ ������������
					if (data[i].dist < MaxDistance - 0.1)
					{
						data[i].dist *= cos(st);
					}
				}

			}
			for (int j = 0; j < 4; j++)
			{
				if (IsCrossing(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y,
					dataLineBot[j].begin.x, dataLineBot[j].begin.y, dataLineBot[j].end.x, dataLineBot[j].end.y))
				{
					Data oneData;
					oneData.acrossPoint = GetCrossVector(point, data[i].acrossPoint, dataLineBot[j].begin, dataLineBot[j].end);
					data[i].acrossPoint = oneData.acrossPoint;
					data[i].type = BOT;
				}
				data[i].dist = distance(point.x, point.y, data[i].acrossPoint.x, data[i].acrossPoint.y);
				// �������� �� ������� ���� ����� ����������� �� ������ ������������
				if (data[i].dist < MaxDistance - 0.1)
				{
					data[i].dist *= cos(st);
				}

			}

			st += StepOfField;// �������� ���������� ���
		}



	}
	void Draw(RenderWindow& window)// ������� ���������
	{
		// ������ ������� ���
		RectangleShape rectangle(Vector2f(screenWidth, screenHeigth / 2));
		CircleShape shape;
		rectangle.setFillColor(Color(128, 128, 255));
		RectangleShape rectEdge(Vector2f(3, 3));
		rectEdge.setFillColor(Color(255, 128, 0));
		rectangle.setPosition(1, 1);
		window.draw(rectangle);
		// ������ ������ ���
		rectangle.setFillColor(Color(255, 128, 128));
		rectangle.setPosition(1, screenHeigth / 2 + 1);
		window.draw(rectangle);

		for (int i = 0; i < amountLines; i++)
		{
			float dist = data[i].dist;// *cos((pi));
			if (dist < MaxDistance - 0.1)
			{

				float lineHeight = dist * 0.5;
				if (data[i].color == Color::Green)// ������ ����� ����� ���� ��� �� ���� ������� �����
				{
					DrawLine((int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2 - 1000 / lineHeight,
						(int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2 + 1000 / lineHeight,
						Color(200 - lineHeight / 2, 200 - lineHeight / 2, 200 - lineHeight / 2), window);
				}
				else
				{	// ������ ����� ����� ���� ��� ���� �����
					DrawLine((int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2 - 1000 / lineHeight,
						(int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2 + 1000 / lineHeight,
						Color::White, window);
				}

			}
			if (data[i].type == BOT)
			{
				float lineHeight = dist * 0.5;
				DrawLine((int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2 - 1000 / lineHeight,
					(int)(screenWidth / (float)amountLines * (amountLines - i)), screenHeigth / 2 + 1000 / lineHeight,
					Color::Red, window);
			}
			if (i <= amountLines / 2 + 15 && i >= amountLines / 2 - 15)
			{
				// ������ ����� �� ����� ������������ ���� ������� �����
				DrawLine(point.x / 4, point.y / 4, data[i].acrossPoint.x / 4, data[i].acrossPoint.y / 4, Color::Red, window);
			}
			else if (data[i].type == BOT)
			{
				DrawLine(point.x / 4, point.y / 4, data[i].acrossPoint.x / 4, data[i].acrossPoint.y / 4, Color::Blue, window);
			}
			else
			{
				// ������ ������ ����� ��� ����� �����
				DrawLine(point.x / 4, point.y / 4, data[i].acrossPoint.x / 4, data[i].acrossPoint.y / 4, Color::Yellow, window);
			}

		}
		// ������ ����� ������� ��������
		Vector2f line1 = point, line2 = point;
		float realDir = dir;
		realDir = realDir * 180 / pi;

		line1.y = 120 * cos(pi * (realDir + 90) / (float)180) + point.y;
		line1.x = 120 * sin(pi * (realDir + 90) / (float)180) + point.x;

		line2.y = 120 * cos(pi * (realDir - 90) / (float)180) + point.y;
		line2.x = 120 * sin(pi * (realDir - 90) / (float)180) + point.x;
		DrawLine(point.x / 4, point.y / 4, line1.x / 4, line1.y / 4, Color::Green, window);
		DrawLine(point.x / 4, point.y / 4, line2.x / 4, line2.y / 4, Color::Green, window);
	}
	void DrawLineBot(RenderWindow &window)
	{
		DrawLine(dataLineBot[0].begin.x / 4, dataLineBot[0].begin.y / 4, dataLineBot[0].end.x / 4, dataLineBot[0].end.y / 4, Color::Blue, window);
		DrawLine(dataLineBot[1].begin.x / 4, dataLineBot[1].begin.y / 4, dataLineBot[1].end.x / 4, dataLineBot[1].end.y / 4, Color::Blue, window);
		DrawLine(dataLineBot[2].begin.x / 4, dataLineBot[2].begin.y / 4, dataLineBot[2].end.x / 4, dataLineBot[2].end.y / 4, Color::Blue, window);
		DrawLine(dataLineBot[3].begin.x / 4, dataLineBot[3].begin.y / 4, dataLineBot[3].end.x / 4, dataLineBot[3].end.y / 4, Color::Blue, window);
	}
}
;
