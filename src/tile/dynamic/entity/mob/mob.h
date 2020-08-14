#ifndef MOB_H
#define MOB_H

#include "entity.h"

class Mob : public Entity {

public:

    Mob(int type_, float x_, float y_);
    ~Mob();

};

#endif
