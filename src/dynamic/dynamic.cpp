#include "dynamic.h"

Dynamic::Dynamic(){
    dir = NORTH;
}

Dynamic::~Dynamic(){}

olc::vf2d Dynamic::getPosition() const {
    return pos;
}

olc::vf2d Dynamic::getVelocity() const {
    return vel;
}

int Dynamic::getSpeed() const {
    return speed;
}

direction Dynamic::getDir() const {
    return dir;
}

int Dynamic::getSize() const {
    return size;
}

void Dynamic::setPosition(float x_, float y_){
    pos = {x_ , y_};
}

void Dynamic::setVelocity(float vx_, float vy_){
    vel = {vx_ , vy_};
}

void Dynamic::setSpeed(int speed_){
    speed = speed_;
}

void Dynamic::setDir(direction dir_){
    dir = dir_;
}

void Dynamic::setSize(int size_){
    size = size_;
}