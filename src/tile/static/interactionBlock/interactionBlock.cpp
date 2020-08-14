
#include "interactionBlock.h"

InteractionBlock::InteractionBlock(){}
InteractionBlock::~InteractionBlock(){}

Key::Key(int type_){
    tileType = type_;
    solidTile = true;
}

Key::~Key(){}

Door::Door(int type_){
    tileType = type_;
    solidTile = true;
}
Door::~Door(){}

Consumable::Consumable(int type_){
    tileType = type_;
    solidTile = true;
}
Consumable::~Consumable(){}

Spawner::Spawner(int type_){
    tileType = type_;
    solidTile = true;
}
Spawner::~Spawner(){}

Exit::Exit(int type_){
    
    tileType = type_;

    // type 10 is a hidden exit 
    // this will change if the block is attacked!
    if(type_ == 32)
        solidTile = false;

    //type 11 is the main exit
    // else if(type_ == 30)
    //     SolidTile = true;

}
Exit::~Exit(){}



