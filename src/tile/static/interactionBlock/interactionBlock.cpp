
#include "interactionBlock.h"

InteractionBlock::InteractionBlock(){}
// InteractionBlock::~InteractionBlock(){}

Key::Key(int type_){
    tileType = type_;
    solidTile = true;
}

// Key::~Key(){}

Door::Door(int type_){
    tileType = type_;
    solidTile = true;

    // if(type_ == 21)
    // {
    //     // vert door

    //     // Door::setSize(  );

    // }else if(type_ == 22){

    // }

}
// Door::~Door(){}

Consumable::Consumable(int type_){
    tileType = type_;
    solidTile = true;
}
// Consumable::~Consumable(){}

Spawner::Spawner(int type_){
    tileType = type_;
    solidTile = true;
}
// Spawner::~Spawner(){}

Exit::Exit(int type_){
    
    tileType = type_;

    // this will change if the block is attacked!
    if(type_ == 32)
        solidTile = false;

    else if(type_ == 33)
        solidTile = true;

}
// Exit::~Exit(){}



