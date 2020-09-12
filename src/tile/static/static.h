
#ifndef STATIC_H
#define STATIC_H

#include "tile.h"

class Static : public Tile {

public:

    Static();
    // ~Static();

};

class Wall : public Static {

public:

    Wall(int type_);
    // ~Wall();

};

class Floor : public Static {

public:

    Floor();
    // ~Floor();

};

#endif