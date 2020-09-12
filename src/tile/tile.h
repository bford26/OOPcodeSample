#ifndef TILE_H
#define TILE_H

#include "olcPixelGameEngine.h"

class Tile {

public:

	Tile();
	// ~Tile();

public:

	/**
	* Simple getter
	* @return pos
	*/
	olc::vf2d getPosition() const;

	/**
	* Simple getter
	* @return size vector
	*/
	olc::vf2d getSize() const;

	/**
	* Simple getter
	* @return tileType
	*/
	int getType() const;

	/**
	* Simple getter
	* @return solidTile
	*/
	bool getSolidTile() const;

	/**
	* Simple getter
	* @return id
	*/
	int getId() const;

	/**
	* sets object position variable pos {x_, y_}
	*/
	void setPosition(float x_, float y_);

	/**
	* overloaded setPosition using olc vector
	*/
	void setPosition(olc::vf2d pos_);

	/**
	* sets the width and height of the object
	*/
	void setSize(olc::vf2d size_);

	/**
	* sets the tileType variable
	*/
	void setType(int type_);

	/**
	* sets solidTile bool
	*/
	void setSolidTile(bool EntvsTile_);

	/**
	* sets the tile identifier
	*/
	void setId(int id_);

public:

	// virtual void Interaction();

protected:

	/**
	* object level/world position 
	*/
	olc::vf2d pos;

	/**
	* defaulted width and height size vector
	*/
	olc::vf2d size = { 16.0f , 16.0f };

	/**
	* indicator used to determine if an dynamic object can ignore the collision
	*/
	bool solidTile = false;

	/**
	* Used for identifying what interactions should happen and sprite should be rendered
	*/
	int tileType;
	
	/**
	* Used for distiguishing objects with the same tileType
	*/
	int id;

};

#endif
