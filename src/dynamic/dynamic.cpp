#include "dynamic.h"

Dynamic::Dynamic(){
    dir = direction::E;
    size = {15.0f, 15.0f};
}

Dynamic::~Dynamic(){}

olc::vf2d Dynamic::getVelocity() const {
    return vel;
}

int Dynamic::getSpeed() const {
    return speed;
}

direction Dynamic::getDir() const {
    return dir;
}

int Dynamic::getId() const {
    return id;
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

void Dynamic::setId(int id_){
    id = id_;
}