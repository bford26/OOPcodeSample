#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

/**
* @brief Defines all the items of a player: name, defence attirbutes, shot and magic power, and treasure, keyValue and potion count.
*/
class Player : public Entity {

public:

    Player();
    // ~Player();

public:

    /**
    * @brief Simple getter.
    * @return name
    */
    std::string getName() const;
    
    /**
    * @brief Simple getter.
    * @return def
    */
    int getDefence() const;
    
    /**
    * @brief Simple getter.
    * @return shotPwr
    */
    int getShotPwr() const;
    
    /**
    * @brief Simple getter.
    * @return magicPwr
    */
    int getMagicPwr() const;

    
    /**
    * @brief sets player name
    */
    void setName(std::string name_);
    
    /**
    * @brief sets def variable
    */
    void setDefence(int def_);
    
    /**
    * @brief sets shotPwr variable
    */
    void setShotPwr(int pwr_);
    
    /**
    * @brief set magicPwr variable
    */
    void setMagicPwr(int pwr_);

private:

    /**
    * @brief player name
    */
    std::string name;

    /**
    * @brief defence for attacks made on player
    */
    int def;

    /**
    * @brief determines projectile strength
    */
    int shotPwr;

    /**
    * @brief determines ability stength uses potions
    */
    int magicPwr;

public:

    /**
    * @brief tresure count, used for increasing hp
    */
    int treasure;
    
    /**
    * @brief holds how many keys player has to open doors
    */
    int keyValue;
    
    /**
    * @brief holds how many potions the player has left
    */
    int potions;

};


#endif