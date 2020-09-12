#ifndef ENTITY_H
#define ENTITY_H

#include "dynamic.h"


/**
* @brief introduces health and fight power, and will describe players and mobs more aptly
*/
class Entity : public Dynamic {

public:

    Entity();
    // ~Entity();

public:

    /**
    * @brief Simple getter.
    * @return hp
    */
    int getHp() const;
    
    /**
    * @brief Simple getter.
    * @return fightPwr
    */
    int getFightPwr() const;

    /**
    * @brief setter for the hp 
    */
    void setHp(int hp_);

    /**
    * @brief setter for the fightPwr
    */
    void setFightPwr(int pwr_);

private:

    /**
    * @brief The health points for the entity
    */
    int hp;

    /**
    * @brief Affects the raw damage done by colliding with an non-friendly entity
    */
    int fightPwr;
    
};

#endif