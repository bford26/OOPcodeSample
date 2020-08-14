#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

class Player : public Entity {

public:

    Player();
    ~Player();

public:

    // getters
    std::string getName() const;
    int getDefence() const;
    int getShotPwr() const;
    int getMagicPwr() const;

    // setters
    void setName(std::string name_);
    void setDefence(int def_);
    void setShotPwr(int pwr_);
    void setMagicPwr(int pwr_);


private:

    std::string name;
    int def;
    int shotPwr, magicPwr;

public:

    int treasure;
    int keyValue;
    int potions;

};


#endif