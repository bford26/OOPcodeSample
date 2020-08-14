
#ifndef STATIC_H
#define STATIC_H

#include "tile.h"
#include "olcPixelGameEngine.h"

class Static : public Tile {

public:

    Static();
    ~Static();

public:

    // getters
    olc::vi2d getPosition() const;
    int getType() const;

    // setters
    void setPosition(int x_, int y_);
    void setPosition(olc::vi2d pos_);

private:
    // will hold the position for the tile in the world space
    olc::vi2d pos;

};

class Wall : public Static {

public:

    Wall(int type_);
    ~Wall();

};

class Floor : public Static {

public:

    Floor();
    ~Floor();

};

#endif