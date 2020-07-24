
#include "interactionBlock.h"

InteractionBlock::InteractionBlock(){}
InteractionBlock::~InteractionBlock(){}
void InteractionBlock::interact(){}

Key::Key(){
    tileType = 2;
    SolidTile = true;
}

Key::~Key(){}

Door::Door(int type_){
    tileType = type_;
    SolidTile = true;
}
Door::~Door(){}

Consumable::Consumable(int type_){
    tileType = type_;
    SolidTile = true;
}
Consumable::~Consumable(){}

Spawner::Spawner(int type_){
    tileType = type_;
    SolidTile = true;
}
Spawner::~Spawner(){}

Exit::Exit(int type_){
    
    tileType = type_;

    // type 10 is a hidden exit 
    // this will change if the block is attacked!
    if(type_ == 10)
        SolidTile = false;

    //type 11 is the main exit
    else if(type_ == 11)
        SolidTile = true;

}
Exit::~Exit(){}



