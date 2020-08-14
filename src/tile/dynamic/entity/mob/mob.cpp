#include "mob.h"

Mob::Mob(int type_, float x_, float y_){
    Dynamic::setType(type_);
    Dynamic::setPosition(x_ , y_);

    if(type_ == 35)
    {   

        Entity::setHp(100);

    }else if(type_ == 36){

        Entity::setHp(150);

    }


    Dynamic::setSpeed(50);
}

Mob::~Mob(){}

