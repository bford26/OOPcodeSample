
#ifndef RESOURCES_H
#define RESOURCES_H

#include<map>
#pragma once
#include "olcPixelGameEngine.h"


/*
This is a implementation of a singleton in c++, to load the reasources for the "game" once
*/

class Resources {

private:

    // making constructors private ensures the user will not be making copies or multiple instances of this class, which would be pointless
    Resources();
    // ~Resources();

public:

    static Resources& get(){
        static Resources self;
        return self;
    }

    // here we delete the copy constructor, so that we wont have multiple instantces of a SINGLETON!
    Resources(Resources const&) = delete;
    // deleteing the "equals" operator makes sure that we can not in anyway be copied
    void operator=(Resources const&) = delete;

    olc::Sprite* getSprite(std::string name);

    void loadSprites();


private:
    std::map<std::string, olc::Sprite*> stringToSpriteMap;

};

#endif
