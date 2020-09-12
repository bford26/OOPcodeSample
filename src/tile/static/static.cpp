
#include "static.h"

Static::Static(){}
// Static::~Static(){}

Wall::Wall(int type_){
    
    tileType = type_;
    solidTile = true;
}

// Wall::~Wall(){}

Floor::Floor(){

    tileType = 19;
    solidTile = false;
}

// Floor::~Floor(){}
