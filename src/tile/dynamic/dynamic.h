#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "olcPixelGameEngine.h"
#include "tile.h"

/**
* @brief direction enum class is used for sprite orientation
*/
enum class direction {
	N, E, S, W,
	NE, NW, SE, SW,
	NONE,
};

/**
* @brief Dynamic class introduces velocity, speed, and direction. Will be used to describe players, projectiles, and mobs. All objects that move.
*/
class Dynamic : public Tile {

public:

	Dynamic();
	//~Dynamic();

public:

	/**
	* @brief Simple getter.
	* @return vel
	*/
	olc::vf2d getVelocity() const;

	/**
	* @brief Simple getter.
	* @return speed
	*/
	int getSpeed() const;

	/**
	* @brief Simple getter.
	* @return dir
	*/
	direction getDir() const;

	/**
	* @brief Simple getter.
	* @return id
	*/
	int getId() const;

	/**
	* @brief sets object velocity variable vel {vx_, vy_}
	*/
	void setVelocity(float vx_, float vy_);

	/**
	* @brief overloaded setVelocity using olc vector
	*/
	void setVelocity(olc::vf2d vel_);

	/**
	* @brief sets the speed variable
	*/
	void setSpeed(int speed_);

	/**
	* @brief sets the dir variable
	*/
	void setDir(direction dir_);

	/**
	* @brief sets the id variable
	*/
	void setId(int id_);


public:

	/**
	* @brief Takes the object dir variable, and creates a vector in that direction (i.e. east : {1,0}, north : {0,1}, NW : {-1, 1} ). Notice they are not normalized vectors.
	* @return an olc vector in the direction of the dir variable
	*/
	olc::vf2d getDirectionVector();

	/**
	* @brief Takes the object vel variable, and determines the cardinale direction.
	* @return the direction in which the velocity is pointing
	*/
	direction getVelocityDirection();

	/**
	* @brief moves the object in the direction of the velocity by vel*fTimeStep, and sets the object direction based on the velocity.
	* @param fTimeStep the wall time that will be used to advance the object position
	*/
	void update(float fTimeStep);

private:

	/**
	* @brief olc float vector for the velocity
	*/
	olc::vf2d vel;

	/**
	* @brief object speed, used to turn a velocity unit vector into a properly size velocity vector
	*/
	int speed;

	/**
	* @brief object direction to determine which sprite to render
	*/
	direction dir;

	/**
	* @brief Used to distingish different objects with the same tileType
	*/
	int id;

public:

	/**
	* @brief if the object is dead (i.e. dead == true) then it will not be considered in collisions or rendering.
	*/
	bool dead = false;

};

#endif