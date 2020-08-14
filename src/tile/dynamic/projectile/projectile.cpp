#include "projectile.h"

Proj::Proj(int type_){

    if(type_ == 37){
        Dynamic::setSpeed(110);
    }
    
    Dynamic::setType(type_);

    Dynamic::setSize(  { 3 , 8}  );
}

Proj::~Proj(){}
