#include "tile.h"

Tile::Tile(){}
Tile::~Tile(){}


olc::vf2d Tile::getPosition(){
    return pos;
}

int Tile::getSize() const {
    return size;
}

int Tile::getType() const {
    return tileType;
}

bool Tile::getSolidTile() const {
    return SolidTile;
}

void Tile::setPosition(float x_, float y_){
    pos = { x_ ,  y_ };
}

void Tile::setPosition(olc::vf2d pos_){
    pos = pos_;
}

void Tile::setSize(int size_){
    size = size_;
}

void Tile::setType(int type_){
    tileType = type_;
}

void Tile::setSolidTile(bool EntvsTile_){
    SolidTile = EntvsTile_;
}






