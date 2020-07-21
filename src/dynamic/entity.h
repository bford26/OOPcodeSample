#ifndef ENTITY_H
#define ENTITY_H

#include "dynamic.h"

class Entity : public Dynamic {

public:

    Entity();
    ~Entity();

public:

    // getters
    int getHp() const;
    int getFightPwr() const;
    int getType() const;

    // setters
    void setHp(int hp_);
    void setFightPwr(int pwr_);
    void setType(char type_);

private:

    int hp;
    int fightPwr;
    int eType;
    
};

#endif