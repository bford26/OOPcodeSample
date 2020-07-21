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
  olc::vf2d getPosition();
  int getSize() const;
  int getType() const;
  bool getSolidTile() const;

  // setters
  void setPosition(float x_, float y_);
  void setPosition(olc::vf2d pos_);
  void setSize(int size_);
  void setType(int type_);
  void setSolidTile(bool EntvsTile_);

private:

  int size = 16;

protected:

  olc::vf2d pos;
  bool SolidTile = false;
  int tileType;

public:
  // these will be used for edges and collision detection
  int edge_id[4];
  bool edge_exist[4];

};

#endif
