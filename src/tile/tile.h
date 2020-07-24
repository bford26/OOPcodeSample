#ifndef TILE_H
#define TILE_H

#include "olcPixelGameEngine.h"

// THESE WILL BE USEd to index the edge ids and exist arrays
#define eNORTH 0
#define eEAST 1
#define eSOUTH 2
#define eWEST 3

struct sEdge {

  // an edge object will hold 4 floats : start and end xy values.
  olc::vf2d start, end;

};


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

protected:

  olc::vf2d pos;
  olc::vf2d size = { 16.0f , 16.0f };
  bool SolidTile = false;
  int tileType;

};

#endif
