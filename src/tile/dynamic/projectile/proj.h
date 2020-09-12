#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "dynamic.h"

/**
* @brief just a sub type of dynamic, but has useful constructor
*/
class Proj : public Dynamic {

public:

    Proj(int type_);
    // ~Proj();
    
};


#endif