
#include "static.h"

Static::Static(){}
Static::~Static(){}

olc::vi2d Static::getPosition() const {
    return pos;
}

int Static::getType() const {
    return tileType;
}

void Static::setPosition(int x_, int y_){
    pos = { x_ , y_ };
}

void Static::setPosition(olc::vi2d pos_){
    pos = { pos_.x , pos_.y };
}

Wall::Wall(int type_){
    
    tileType = type_;
    SolidTile = true;
}

Wall::~Wall(){}

Floor::Floor(){

    tileType = 1;
    SolidTile = false;
}
Floor::~Floor(){}
