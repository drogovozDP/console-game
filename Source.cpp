
#include <iostream>
#include <conio.h>
#include <vector>
#include <windows.h> 
#include <algorithm> 

using namespace std;

int max_x = 60;
int max_y = 29;


void setcur(int y, int x)//������� ������� � � � ���� �������� ���� ����.
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

class Entity
{
protected:
	int this_x = 0;
	int this_y = 0;
	char character;
	bool life;
	vector<vector<Entity*>>*map;
	Entity(int x, int y, char character, bool life) : 
		this_x(x), this_y(y), 
		character(character), life(life) {}
public:
	void show()
	{
		cout << character;
	}
	char letter() { return character; }
	int x() { return this_x; };
	int y() { return this_y; };
	virtual void collision(Entity* target) = 0;
	virtual void die() = 0;

};

vector<vector<Entity*>>field;



class Cell :public Entity
{
public:
	Cell() : Entity(0, 0, '.', false)
	{
		character = '.';
	}
	void collision(Entity* target)
	{
	}
	void die()
	{
	}

};

class Barrier : public Entity
{
public:
	Barrier() : Entity(0, 0, '#', false)
	{}

	void collision(Entity* target)
	{
	}
	void die()
	{
	}

};

class Hero :public Entity
{
	vector<vector<Entity*>> &map;
public:
	Hero(int y_pos, int x_pos, char player, vector<vector<Entity*>> &map): 
		Entity(x_pos, y_pos, player, true),
		map(map)
	{}
	void die()
	{
		character = '.';
		this_y = 1000;
		this_x = 1000;
		life = false;
	}

	bool step(int y, int x)
	{
		if (map[y][x]->letter() != '.') return false;
		else return true;
	}

	void move(int dy, int dx)
	{
		if (!life) return;
		if (!step(this_y + dy, this_x + dx)) return;
		setcur(this_y, this_x);
		cout << '.';
		setcur(this_y + dy, this_x + dx);
		cout << character;
		setcur(max_y, max_x);
		swap(map[this_y][this_x], map[this_y + dy][this_x + dx]);
		this_x += dx;
		this_y += dy;
	}

	void collision(Entity* target)
	{
		if ((this_y == target->y()) && (this_x == target->x()) && (target->letter() != '$')) target->die();
	}
};

class Enemy :public Entity
{
protected:
	int* path;
	int dy, dx = 0;
public:
	Enemy(int y_pos, int x_pos, int hero_count) : Entity(x_pos, y_pos, '$', true), path(new int[hero_count])
	{
	}

	void move(vector<Entity*>& players, vector<vector<Entity*>>& way)
	{
		for (auto i = 0U; i < players.size(); i++)
		{
			path[i] = int(pow(players[i]->y() - this_y, 2) + pow(players[i]->x() - this_x, 2));
		}
		for (int i = 0; i < players.size(); i++)
			for (int j = 0; j < players.size(); j++)
				if (path[i] < path[j])
				{
					swap(players[i], players[j]);
					swap(path[i], path[j]);
				}

		if (abs(this_y - players[0]->y()) > abs(this_x - players[0]->x()))
		{
			if (this_y - players[0]->y() > 0) dy = -1;
			else dy = 1;
		}
		else
		{
			if (this_x - players[0]->x() > 0) dx = -1;
			else dx = 1;
		}
		setcur(this_y, this_x);
		cout << '.';
		setcur(this_y + dy, this_x + dx);
		cout << character;
		setcur(max_y, max_x);
		swap(way[this_y + dy][this_x + dx], way[this_y][this_x]);
		this_y += dy;
		this_x += dx;
		dy = 0;
		dx = 0;
	}

	void die()
	{
		cout << "die ";
	}

	void collision(Entity *target)
	{
		if ((this_y == target->y()) && (this_x == target->x()))
		{
			target->die();
		}
	}

};
/*
class Bullet: public Entity
{
public:
	Bullet(int dy, int dx, )

	void die()
	{

	}

	void collision(Entity * target)
	{

	}
};*/

int main()
{
	vector<vector<Entity*>>field(max_y);
	Hero P_one(6, 5, '1', field);
	Hero P_two(25, 25, '2', field);

	vector<Entity*>players;
	players.push_back(&P_one);
	players.push_back(&P_two);

	vector<Entity*> world;
	Enemy Mob(13, 5, 2);
	world.push_back(&P_one);
	world.push_back(&P_two);
	world.push_back(&Mob);
	Cell dot;
	Barrier not_dot;


	for (int i = 0; i < max_y; i++)
	{
		for (int j = 0; j < max_x; j++)
		{
			field[i].push_back(&dot);
		}
	}
	for (int i = 0; i < max_x; i++) field[0][i] = &not_dot;//top
	for (int i = 0; i < max_x; i++) field[max_y - 1][i] = &not_dot;//bot
	for (int i = 0; i < max_y; i++) field[i][0] = &not_dot;//left
	for (int i = 0; i < max_y; i++) field[i][max_x - 1] = &not_dot;//right


	field[P_one.y()][P_one.x()] = &P_one;
	field[P_two.y()][P_two.x()] = &P_two;
	field[Mob.y()][Mob.x()] = &Mob;

	for (int i = 0; i < max_y; i++)
	{
		for (int j = 0; j < max_x; j++)
		{
			field[i][j]->show();
		}
		cout << endl;
	}

	char a = ' ';
	int time = 300;
	int counting = 0;
	int timer = 0;
	while (true)
	{
		if (_kbhit())
		{
			char move = _getch();
			if (move == 'w') P_one.move(-1, 0);
			if (move == 's') P_one.move(1, 0);
			if (move == 'a') P_one.move(0, -1);
			if (move == 'd') P_one.move(0, 1);

			if (move == 'i') P_two.move(-1, 0);
			if (move == 'k') P_two.move(1, 0);
			if (move == 'j') P_two.move(0, -1);
			if (move == 'l') P_two.move(0, 1);
		}
		if (timer == 3)//�������� ������
		{
			Mob.move(players, field);
			timer = 0;
		}
		else timer++;//�������� �����
		for (int i = 0; i < world.size(); i++)
			for (int j = 0; j < world.size(); j++)
				if (i != j) world[i]->collision(world[j]);
		Sleep(100);

	}

	return 0;
}
