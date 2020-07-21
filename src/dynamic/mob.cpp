#include "mob.h"

Mob::Mob(int type_, float x_, float y_){
    this->setType(type_);
    this->setPosition(x_ , y_);
}

Mob::~Mob(){}

bool Mob::getStatus() const {
    return dead;
}

void Mob::setStatus(bool status_){
    dead = status_;
}
