#ifndef GAME_CLASS_H
#define GAME_CLASS_H

#include "olcPixelGameEngine.h"
// Dynamic and entity includes
#include "player.h"
#include "mob.h"
#include "projectile.h"
// tile and static includes
#include "interactionBlock.h"
// singlton include for resource management
#include "resources.h"

class Gauntlet : public olc::PixelGameEngine {

public:

    Gauntlet();

private:

    int vertTiles;
    int horTiles;
    
    // gameObjects will store a map of the level, so for interactions
    // and displaying sprites this flattened array can be used
    Tile **gameObjects = nullptr;

    // decal are GPU based sprites, used for high performance.
    olc::Decal **decalList = nullptr;
    int decalNum = 10;

    // used for display info
    olc::Decal *infoDecal1 = nullptr, *infoDecal2 = nullptr;

    int nLevel = 0;
    int ExitGame = 10;
    bool bGameLoop = true;

private:

    // this is basically a way to take advantage of ploymorphism
    // std::vector<Dynamic> dynList;

    std::vector<Mob> mobList;
    std::vector<Proj> projList;
    Player *p1;

public:

    // collision detection 
    bool PointVsTile(const olc::vf2d& p, const Tile* pTile);
    bool TileVsTile(const Tile* pt1, const Tile* pt2);
    bool RayVsTile(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const Tile* target, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near);
    bool DynamicVsTile(const Dynamic* pDyn, const float fTimeStep, const Tile& pTile, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time);

    bool ResolveCollision(Dynamic* pDyn, const float fTimeStep, Tile* pTile, int index);
    bool ResolveCollision(Dynamic* pDyn, const float fTimeStep, Mob  *m);
    bool ResolveCollision(Dynamic* pDyn, const float fTimeStep, Proj *p);




public:

    bool OnUserCreate() override;
    bool OnUserUpdate(float fElaspsedTime) override;
    bool OnUserDestroy() override;

public:

    void updatePlayer(float fElapsedTime);
    void updateMobs(float fElapsedTime);
    void updateProjectiles(float fElapsedTime);

    void DrawDecals();

    void DrawLevel();
    void DrawPlayer();
    void DrawEntities();

    bool loadLevel(int index);

    void utilFunc(float fElapsedTime);


};

#endif

