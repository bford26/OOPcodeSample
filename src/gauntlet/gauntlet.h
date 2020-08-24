#ifndef GAME_CLASS_H
#define GAME_CLASS_H

// #include "olcPixelGameEngine.h"
// Dynamic and entity includes
#include "player.h"
#include "mob.h"
#include "projectile.h"
// tile and static includes
#include "interactionBlock.h"
// singlton include for resource management
#include "resources.h"


enum GameState {

    START, GAME, PAUSED, OVER,

};



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
    olc::Decal *infoDecal1 = nullptr, *infoDecal2 = nullptr, *outlineDecal = nullptr;

    int nLevel = 0;
    int ExitGameCounter = 1000;
    bool bGameLoop = true;
    olc::vf2d exitPos = {0.0f, 0.0f};

    olc::Decal *utilDecal = nullptr;

    GameState gamestate;
    int PauseChoice = -1;


    float spawnRate = 2.0f;

    float lastTime, curTime;

    // which frame to use
    bool frameBool = false;

    bool drawOutline = false;
    bool oneSecond = false;

    bool projShot = false;


    using clock = std::chrono::high_resolution_clock;
    using ms = std::chrono::milliseconds;
    // using std::chrono::duration_cast;

    clock::time_point cur_time = clock::now(), prev_time = clock::now();
    uint64_t time_diff;


private:aw

    // input timing
    clock::time_point Wpress, Apress, Spress, Dpress, minTime;



private:

    // this is basically a way to take advantage of ploymorphism
    // std::vector<Dynamic> dynList;

    std::vector<Mob> mobList;
    std::vector<Proj> projList;
    Player *p1;

public:

    // collision detection 

    bool TileVsTile(const Tile* pt1, const Tile* pt2);

    bool RayVsTile(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const Tile* target, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near);
    bool RayVsDynamic(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const Dynamic* target, const float fTimeStep, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near);

    // determine 
    bool DynamicVsTile(const Dynamic* pDyn, const float fTimeStep, const Tile& pTile, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time);
    bool EntityVsDynamic(const Entity* pEnt, const float fTimeStep, const Dynamic& pDyn, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time);
    

    bool ResColl_DynamicVsTile(Dynamic* pDyn, const float fTimeStep, Tile* pTile, int index);
    bool ResColl_EntityVsDynamic(Entity* pEnt, const float fTimeStep, Dynamic* pDyn);



public:

    bool OnUserCreate() override;
    bool OnUserUpdate(float fElaspsedTime) override;
    bool OnUserDestroy() override;

public:

    // game loop functions
    void playerCollisions(float fElapsedTime);
    void mobCollisions(float fElapsedTime);
    void projCollisions(float fElapsedTime);
    
    void updatePlayer(float fElapsedTime);
    void updateProjectiles(float fElapsedTime);
    void updateMobs(float fElapsedTime);
    void updateTiles(float fElapsedTime);


    // drawing functionality
    void DrawLevel();
    void DrawEntities();


    // io for loading the current game level choosen
    bool loadLevel(int index);

};

#endif

