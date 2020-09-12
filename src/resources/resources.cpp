
#include "resources.h"

Resources::Resources(){}
// Resources::~Resources(){}

olc::Sprite* Resources::getSprite(std::string name)
{
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

    // this will hold all the level sprites; walls, floor, exit, etc..
    load("lvlSheet", "data/sprites/static/spriteSheet.png");
    load("player", "data/sprites/dynamic/questor.png");
    load("p1Proj", "data/sprites/dynamic/questor_proj.png");
    load("ghost", "data/sprites/dynamic/enemy01.png");
    load("demon", "data/sprites/dynamic/enemy02.png");
    load("info1", "data/sprites/static/info1.png");
    load("info2", "data/sprites/static/info2.png");
    
    load("startScreen", "data/sprites/static/startScreen.png");
    load("pauseScreen", "data/sprites/static/pauseScreen.png");

    // debugging
    load("outline", "data/sprites/static/EntityOutline.png");

}


size_t Resources::getSpriteCount()
{
    return stringToSpriteMap.size();
}



