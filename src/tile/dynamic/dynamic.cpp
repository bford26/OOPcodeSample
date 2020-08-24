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
void Dynamic::setVelocity(olc::vf2d vel_){
    vel = vel_;
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

olc::vf2d Dynamic::getDirectionVector(){

    direction dir = Dynamic::getDir();
    olc::vf2d vec = {0.0f,0.0f};

    if(dir == direction::N){
        vec.x = 0;
        vec.y = -1;

    } else if(dir == direction::E){
        vec.x = 1;
        vec.y = 0;
    
    } else if(dir == direction::S){
        vec.x = 0;
        vec.y = 1;
    
    } else if(dir == direction::W){
        vec.x = -1;
        vec.y = 0;

    } else if(dir == direction::NW){
        vec.x = -1;
        vec.y = -1;

    } else if(dir == direction::NE){
        vec.x = 1;
        vec.y = -1;

    } else if(dir == direction::SW){
        vec.x = -1;
        vec.y = 1;

    } else if(dir == direction::SE){
        vec.x = 1;
        vec.y = 1;
    }

    return vec.norm();

}

direction Dynamic::getVelocityDirection(){

    direction dir = Dynamic::getDir();
    olc::vf2d v = Dynamic::getVelocity().norm();

    if(v.x > 0)
    {
        if(v.y == 0)
            dir = direction::E;
        else if( v.y > 0)
            dir = direction::SE;
        else if( v.y < 0)
            dir = direction::NE;

    }else if( v.x < 0 ){

        if(v.y == 0)
            dir = direction::W;
        else if( v.y > 0)
            dir = direction::SW;
        else if( v.y < 0)
            dir = direction::NW;

    }else{

        if(v.y < 0)
            dir = direction::N;
        else if( v.y > 0)
            dir = direction::S;

    }

    return dir;
}



void Dynamic::update(float fTimeStep)
{
    if(Dynamic::getVelocity().mag() != 0.0f)
    {
        Dynamic::setPosition( Dynamic::getPosition() + Dynamic::getVelocity() * fTimeStep );
        Dynamic::setDir(Dynamic::getVelocityDirection());
    }
}