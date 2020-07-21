#ifndef ACTION_BLOCK_H
#define ACTION_BLOCK_H

#include "static.h"

class InteractionBlock : public Static {

public:

    InteractionBlock();
    ~InteractionBlock();

public:

    virtual void interact();

};

class Key : public InteractionBlock {

public:

    Key();
    ~Key();

};

class Door : public InteractionBlock {

public:

    Door(int type_);
    ~Door();

};

class Consumable : public InteractionBlock {

public:

    Consumable(int type_);
    ~Consumable();

};

class Spawner : public InteractionBlock {

public:

    Spawner(int type_);
    ~Spawner();

};

class Exit : public InteractionBlock {

public:

    Exit(int type_);
    ~Exit();

};

#endif