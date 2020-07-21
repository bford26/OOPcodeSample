#include "entity.h"

Entity::Entity(){}
Entity::~Entity(){}

int Entity::getHp() const {
    return hp;
}

int Entity::getFightPwr() const {
    return fightPwr;
}

int Entity::getType() const {
    return eType;
}

void Entity::setHp(int hp_){
    hp = hp_;
}

void Entity::setFightPwr(int pwr_){
    fightPwr = pwr_;
}

void Entity::setType(char type_){
    eType = type_;
}