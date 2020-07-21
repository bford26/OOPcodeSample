#ifndef MOB_H
#define MOB_H

#include "entity.h"

class Mob : public Entity {

public:

    Mob(int type_, float x_, float y_);
    ~Mob();

public:

    // getters
    
    bool getStatus() const;

    // setters

    void setStatus(bool status_);

private:

    // this is just a flag used to remove dead mobs from game
    bool dead;

};

#endif
