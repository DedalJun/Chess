#include <SFML/Graphics.hpp>
#include <time.h>
#include <math.h>
#include "Connector.hpp"
using namespace sf;

int size = 56;
Vector2f offset(1,1);

Sprite f[32]; //figures
std::string position = "";

int board[8][8] =
{ -1,-2,-3,-4,-5,-3,-2,-1,
-6,-6,-6,-6,-6,-6,-6,-6,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
6, 6, 6, 6, 6, 6, 6, 6,
1, 2, 3, 4, 5, 3, 2, 1 };

bool canIMove(int n, Vector2f oldc, Vector2f newc) {
	if (n >= 8 && n <= 23) n = 8;
	
	int oldx = oldc.x / size;
	int oldy = 7 - oldc.y / size;

	int newx = newc.x / size;
	int newy = 7 - newc.y / size;

	switch (n)
	{
	//rook
	case 0:
	case 7:
	case 24:
	case 31: {
		if (newx == oldx || newy == oldy) return true;
		else return false;
		break;
	   }
	//horse
	case 1:
	case 6:
	case 25:
	case 30: {
		if (((abs(newx - oldx) == 2) && (abs(newy - oldy) == 1)) || ((abs(newx - oldx) == 1) && (abs(newy - oldy) == 2)))
			return true;
		else return false;
		break;
	   }
	//bishop
	case 2:
	case 5:
	case 26:
	case 29: {
		if (abs(newx - oldx) == abs(newy - oldy)) return true;
		else return false;
		break;
	}
	//pawn
	case 8: {
		if (abs(newy - oldy) <= 2 && abs(newx - oldx) <= 1) return true;
		else return false;
	}
	default:
		return true;
		break;
	}
}

std::string toChessNote(Vector2f p)
{
	std::string s = "";
	s += char(p.x / size + 97);
	s += char(7 - p.y / size + 49);
	return s;
}

Vector2f toCoord(char a, char b)
{
	int x = int(a) - 97;
	int y = 7 - int(b) + 49;
	return Vector2f(x*size, y*size);
}

void move(std::string str)
{
	Vector2f oldPos = toCoord(str[0], str[1]);
	Vector2f newPos = toCoord(str[2], str[3]);

	for (int i = 0; i<32; i++)
		if (f[i].getPosition() == newPos) f[i].setPosition(-100, -100);

	for (int i = 0; i<32; i++)
		if (f[i].getPosition() == oldPos) f[i].setPosition(newPos);

	//castling       //if the king didn't move
	if (str == "e1g1") if (position.find("e1") == -1) move("h1f1");
	if (str == "e8g8") if (position.find("e8") == -1) move("h8f8");
	if (str == "e1c1") if (position.find("e1") == -1) move("a1d1");
	if (str == "e8c8") if (position.find("e8") == -1) move("a8d8");
}

void loadPosition()
{
	int k = 0;
	for (int i = 0; i<8; i++)
		for (int j = 0; j<8; j++)
		{
			int n = board[i][j];
			if (!n) continue;
			int x = abs(n) - 1;
			int y = n>0 ? 1 : 0;
			f[k].setTextureRect(IntRect(size*x, size*y, size, size));
			f[k].setPosition(size*j, size*i);
			k++;
		}

	for (int i = 0; i<position.length(); i += 5)
		move(position.substr(i, 4));
}


int main()
{
	RenderWindow gameWindow(VideoMode(450, 450), "Chess");

	std::cout << "Welcome player! \n"
		<< "It is chess game! \n"
		<< "You can play with friend or with computer \n"
		<< "If you want to play with computer, press SPACE after every your turn \n"
		<< "Press BACKSPACE for cance last move \n";
	
	Texture t1,t2;
	t1.loadFromFile("img/figures.png");
	t2.loadFromFile("img/board.png");
	Sprite s(t1);
	Sprite board(t2);

	Vector2f oldPosition, newPosition;
	int n = 0;
	std::string str = "";

	bool isMove = false;
	float dx = 0, dy = 0;

	ConnectToEngine("stockfish.exe");

	for (int i = 0; i < 38; i++) f[i].setTexture(t1);

	loadPosition();

	while (gameWindow.isOpen())
	{
		Vector2i pos = Mouse::getPosition(gameWindow) - Vector2i(offset);

		Event e;
		while (gameWindow.pollEvent(e))
		{
			if (e.type == Event::Closed)
				gameWindow.close();

			if (e.type == Event::KeyPressed)
				if (e.key.code == Keyboard::BackSpace)
				{
					if (position.length()>6) position.erase(position.length() - 6, 5); 
					loadPosition();
				}


			if(e.type == Event::MouseButtonPressed)
				if(e.key.code == Mouse::Left)
					for (int i = 0; i<32; i++)
						if (f[i].getGlobalBounds().contains(pos.x, pos.y))
						{
							isMove = true; 
							n = i;
							dx = pos.x - f[i].getPosition().x;
							dy = pos.y - f[i].getPosition().y;
							oldPosition = f[i].getPosition();
						}

			if (e.type == Event::MouseButtonReleased)
				if (e.key.code == Mouse::Left)
				{
					isMove = false;
					Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);
					newPosition = Vector2f(size*int(p.x / size), size*int(p.y / size));

					if (canIMove(n, oldPosition, newPosition))
					{
						str = toChessNote(oldPosition) + toChessNote(newPosition);
						std::cout << "Player - " << str << "\n";
						move(str);
						if (oldPosition != newPosition) position += str + " ";
					}
					else {
						newPosition = oldPosition;
						std::cout << "Sorry, but you can`t move figure here \n";
					}
                    f[n].setPosition(newPosition);
				}
		}

		//comp move
		if (Keyboard::isKeyPressed(Keyboard::Space))
		{
			str = getNextMove(position);
			std::cout << "Computer " << str << "\n";
			oldPosition = toCoord(str[0], str[1]);
			newPosition = toCoord(str[2], str[3]);

			for (int i = 0; i<32; i++) 
				if (f[i].getPosition() == oldPosition) n = i;

			/////animation///////
			for (int k = 0; k<50; k++)
			{
				Vector2f p = newPosition - oldPosition;
				f[n].move(p.x / 50, p.y / 50);
				gameWindow.draw(board);
				for (int i = 0; i<32; i++) f[i].move(offset);
				for (int i = 0; i<32; i++) gameWindow.draw(f[i]); 
				gameWindow.draw(f[n]);
				for (int i = 0; i<32; i++) f[i].move(-offset);
				gameWindow.display();
			}

			move(str);  
			position += str + " ";
			f[n].setPosition(newPosition);
		}

		if (isMove) f[n].setPosition(pos.x - dx, pos.y - dy);

		gameWindow.clear();
		gameWindow.draw(board);
		//for (int i = 0; i<32; i++) f[i].move(offset);
		for (int i = 0; i<32; i++) gameWindow.draw(f[i]); 
		gameWindow.draw(f[n]);
		//for (int i = 0; i<32; i++) f[i].move(-offset);
		gameWindow.display();
	}
	CloseConnection();
	return 0;
}