#pragma once
#include <string>

//Use standard namespace
using namespace std;

class Foot
{
public:
	Foot();
	~Foot();

	int getX();
	int getY();

	void setX(int x);
	void setY(int y);

private:
	int foot_x;
	int foot_y;

	string type;
};
