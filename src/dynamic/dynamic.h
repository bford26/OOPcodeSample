#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "olcPixelGameEngine.h"

enum direction {
    NORTH, EAST, SOUTH, WEST,
};

class Dynamic {

public:

    Dynamic();
    ~Dynamic();

public:

    // getters
    olc::vf2d getPosition() const;
    olc::vf2d getVelocity() const;
    int getSpeed() const;
    direction getDir() const;
    olc::vf2d getSize() const;

    // setters
    void setPosition(float x_, float y_);
    void setVelocity(float vx_, float vy_);
    void setSpeed(int speed_);
    void setDir(direction dir_);
    void setSize(olc::vf2d size_);

private:

    olc::vf2d pos;
    olc::vf2d vel;
    int speed;
    direction dir;
    olc::vf2d size = { 15.0f , 15.0f};

public:

    // for collisions?

};

#endif