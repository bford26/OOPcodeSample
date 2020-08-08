#ifndef TILE_H
#define TILE_H

#include "olcPixelGameEngine.h"

class Tile {

public:

  Tile();
  ~Tile();

public:

  //getters
  olc::vf2d getPosition() const;
  olc::vf2d getSize() const;
  int getType() const;
  bool getSolidTile() const;

  // setters
  void setPosition(float x_, float y_);
  void setPosition(olc::vf2d pos_);
  void setSize(olc::vf2d size_);
  void setType(int type_);
  void setSolidTile(bool EntvsTile_);


public:

  // virtual void Interaction();

protected:

  olc::vf2d pos;
  olc::vf2d size = { 16.0f , 16.0f };
  bool solidTile = false;
  int tileType;




};

#endif
