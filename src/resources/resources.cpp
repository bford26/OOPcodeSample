
#include "resources.h"

Resources::Resources(){}
Resources::~Resources(){}

olc::Sprite* Resources::getSprite(std::string name){
    return stringToSpriteMap[name];
}

void Resources::loadSprites(){
    
    // lambda function used to load all the sprite we will use and attach them to the given string
    auto load = [&](std::string spriteName, std::string fileName){

        olc::Sprite* s = new olc::Sprite(fileName);
        stringToSpriteMap[spriteName] = s;
    
    };


    /* 
    here we load everything we need for the game!

    I should note that this is not an efficient method for creating an entire game. Some levels in a game may require only a handful of the entire resources made for a game, and loading everything at the begining is not advisiable.

    A much smoother method  would load the reasources at the start of each new level/world/arena. Here we load everything at the begining because this is just a demonstration and all these resources are used in the demo level.
    */

    load("wall", "data/sprites/static/wall.png");
    load("floor", "data/sprites/static/floor.png");
    load("key", "data/sprites/static/key.png");
    load("door", "data/sprites/static/door.png");
    load("spawner", "data/sprites/static/spawner.png");
    load("treasure", "data/sprites/static/treasure.png");
    load("player", "data/sprites/static/player.png");
    load("ghost", "data/sprites/static/enemy01.png");
    load("demon", "data/sprites/static/enemy02.png");

}





