#include "player.h"
    
Player::Player(){

    keyValue = 0;
    treasure = 0;
    potions = 0;
    Dynamic::setType(34);
}

// Player::~Player(){}

std::string Player::getName() const {
    return name;
}

int Player::getDefence() const {
    return def;
}

int Player::getShotPwr() const {
    return shotPwr;
}

int Player::getMagicPwr() const {
    return magicPwr;
}

void Player::setName(std::string name_){
    name = name_;
}

void Player::setDefence(int def_){
    def = def_;
}

void Player::setShotPwr(int pwr_){
    shotPwr = pwr_;
}

void Player::setMagicPwr(int pwr_){
    magicPwr = pwr_;
}

