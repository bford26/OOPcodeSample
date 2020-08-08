#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "olcPixelGameEngine.h"
#include "tile.h"

enum direction {
    N, E, S, W,
    NE, NW, SE, SW,
};

class Dynamic : public Tile {

public:

    Dynamic();
    ~Dynamic();

public:

    // getters
    olc::vf2d getVelocity() const;
    int getSpeed() const;
    direction getDir() const;
    int getId() const;

    // setters
    void setVelocity(float vx_, float vy_);
    void setSpeed(int speed_);
    void setDir(direction dir_);
    void setId(int id_);


private:

    olc::vf2d vel;
    int speed;
    direction dir;
    int id;

public:

    // for collisions?
    bool dead =  false;
    
};

#endif