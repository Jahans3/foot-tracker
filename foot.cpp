#include "Foot.h"


Foot::Foot()
{
}


Foot::~Foot()
{
}

int Foot::getX(){
	return Foot::foot_x;
}

int Foot::getY(){
	return Foot::foot_y;
}

void Foot::setX(int x){
	Foot::foot_x = x;
}

void Foot::setY(int y){
	Foot::foot_y = y;
}
