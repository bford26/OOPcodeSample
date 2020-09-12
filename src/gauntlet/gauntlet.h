#ifndef GAME_CLASS_H
#define GAME_CLASS_H

#include "player.h"
#include "mob.h"
#include "proj.h"
#include "interactionBlock.h"
#include "resources.h"


enum class GameState {

    START, GAME, PAUSED, OVER,

};

class Gauntlet : public olc::PixelGameEngine {

public:

    Gauntlet();

private:

    int vertTiles;
    int horTiles;

    Tile **gameObjects = nullptr;

    olc::Decal **decalList = nullptr;
    int decalNum = 5;

    olc::Decal *infoDecal1 = nullptr, *infoDecal2 = nullptr, *outlineDecal = nullptr;
    int nLevel;
    int ExitGameCounter = 1000;

    bool bGameLoop = true;
    olc::vf2d exitPos = {0.0f, 0.0f};
    olc::Decal *utilDecal = nullptr;
    GameState gamestate;
    int PauseChoice = 0;

    bool frameBool = false;
    bool drawOutline = false;
    bool oneSecond = false;
    bool projShot = false;

    using clock = std::chrono::high_resolution_clock;
    using ms = std::chrono::milliseconds;
    clock::time_point cur_time = clock::now(), prev_time = clock::now();
    uint64_t time_diff;

private:

    float screen_bound_TOP, screen_bound_LEFT, screen_bound_RIGHT, screen_bound_BOT;
    float offx, offy;

private:

    // input timing
    clock::time_point Wpress, Apress, Spress, Dpress, minTime;
    clock::time_point Wrelease, Arelease, Srelease, Drelease;

    // update timing
    clock::time_point mob_flicker;    
    clock::time_point mob_last_time, mob_current_time;
    clock::time_point spawnrate;
    clock::time_point tile_last_time, tile_current_time;
    clock::time_point player_flicker;

private:

    std::vector<Mob> mobList;
    std::vector<Proj> projList;
    Player *p1;

public:

    ///@{
    /// @name Unit Collision Functions

    /**
    * utility member function used to determine if the two passed objects have coordinates that overlap
    * 
    * @param pt1 a pointer to a Tile object
    * @param pt2 a pointer to a Tile object
    * 
    * @return true if the tile objects passed overlap, otherwise false
    * 
    */
    bool TileVsTile(const Tile* pt1, const Tile* pt2);

    /**
    * 
    * casts a ray from ray_origin in the given ray_dir, and determines values for contact_point, contact_normal, and t_hit_near; if the ray intersects with the Tile target object
    * 
    * @param ray_origin position vector for the center of an object which will be tested against various Tile objects
    * @param ray_dir the direction in which the object is wanting to move
    * @param target the Tile object which will be tested for a intersection
    * @param contact_point the position vector passed by reference, which will be set to {0,0} unless a intersection was found. otherwise set to the intersection position
    * @param contact_normal the normal vector to the intersection, aka what side of the Tile object was hit by the cast ray first   
    * @param t_hit_near the closest intersection distance, from the cast ray
    * 
    * @return true if a value other than NAN is found for the ray intersection, false for all exceptions 
    */
    bool RayVsTile(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const Tile* target, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near);
    
    /**
    * Determines if the pDyn object will collide with the pTile object. Creates a temp Tile object copied from pTile, but expanded to include the pDyn object size.
    * This is used to so that the contact_point is the point will be the closest the pDyn object can move towards the pTile object without a collision.
    * 
    * @param pDyn pointer to a Dynamic object, typically the Player object and Proj objects
    * @param fTimeStep the time in seconds that will be used to advanced the Dynamic object by its velocity
    * @param pTile pointer to a Tile object, will be used to test if the pDyn object will have a collision
    * @param contact_point the position vector passed by reference, which will be set to {0,0} unless a intersection was found. otherwise set to the intersection position
    * @param contact_normal the normal vector to the intersection, aka what side of the Tile object was hit by the cast ray first 
    * @param contact_time effectively t_hit_near, which is the distance from the object to a intersection from the ray cast at the center of the pDyn object
    * 
    * @return true if the contact_time is between [0,1), false if pDyn velocity is zero or if no intersection from RayVsTile
    */
    bool DynamicVsTile(const Dynamic* pDyn, const float fTimeStep, const Tile& pTile, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time);
    

    /**
    * 
    * Handels all game logic for collisions, which vary depending on pTile type (i.e. walls, keys, and doors behave different to collisions)
    * 
    * @param pDyn pointer to a Dynamic object, typically the Player object and Proj objects
    * @param fTimeStep the time which will be used to advance the pDyn object by its velocity
    * @param pTile a pointer to a Tile object, which has a collision with the pDyn object
    * @param index a integer, giving the index for the pTile object in the gameObjects member variable
    * 
    * @return true when the collision is handel, false when the pTile object passed does not collide with pDyn
    * 
    */
    bool ResColl_DynamicVsTile(Dynamic* pDyn, const float fTimeStep, Tile* pTile, int index);

    /**
    * 
    * Used for checking collisions between players/projectiles and mobs, damages or kills mobs based on type of collision
    * 
    * @param m pointer to Mob object, to determine if anything is required to change
    * @param fTimeStep the time in seconds that will be used to advanced the Dynamic object by its velocity
    * @param pDyn pointer to a Dynamic object, typically the Player object and Proj objects
    * 
    * @return always returns true, not currently important information to track
    * 
    */
    bool Resolve_MobVsDynamic(Mob* m, const float fTimeStep, Dynamic* pDyn);

    ///@}


public:

    ///@{
    /// @name Game Engine Functions

    /**
    * olc PGEX virtual function, which is called prior to the gameloop function OnUserUpdate. Used for loading resources, level data, and init for game state.
    * 
    * @return always true, so that the game will run and enter the loop
    * 
    */
    bool OnUserCreate() override;

    /**
    *
    * olc PGEX virtual function, which is the gameloop: game logic, rendering, and input information is all located within this overwitten function.
    * 
    * Uses a switch statement with the var gamestate, to change the rendering and logic used depending on which state the application is running.
    * 
    * START:
    *   displays start screen, while checking for any keyboard updates, to then enter the GAME game state
    * 
    * GAME:
    *   timing, collision handeling, and fixed updates (resetting the world bounds which are display to screen based on players position)
    *   Rendering level, info, then Entities and projectiles
    *   Gamelogic - updating tiles, player, projectiles, then mobs
    *   gather inputs from user to determine how game should change
    *   
    * PAUSED:
    *   gives option to resume or exit the applicatiion
    * 
    * OVER:
    *   only occurs when player reaches level exit, renders the exit animation
    * 
    * @param fElaspedTime is the elasped wall time from the last OnUserUpdate call and is used for advancing objects by their velocity
    * 
    * @return true while continuning game loop, only false when player chooses to exit game or reaches the level exit
    */
    bool OnUserUpdate(float fElaspsedTime) override;

    /**
    * olc PGEX virtual function, which is the effective Destructor for the Gauntlet class: freeing up any dynamic memory
    * 
    * @returns always true
    */
    bool OnUserDestroy() override;

    ///@}

public:

    ///@{
    /// @name Utility Game Functions
 
    /**
    * Mainly used for determining player velocity and if a player projectile should be created. 
    * 
    * Gathers timing information from keyboard inputs so player can use multiple keys at once without have issues on releasing keys at the same time.
    * 
    * Also determines if the game state will be set to PAUSED, if the escape key is pressed
    * 
    */
    void GatherInputs();

    /**
    * Handels all collisions for the game based on tile type
    * 
    * priority order:
    *   player vs tiles
    *   projs vs tiles
    *   mobs vs player
    *   mobs vs projs
    * 
    * Do not need to check for mobs vs tiles because mobs are controlled by a simple AI which checks for tiles before setting velocity
    * 
    */
    void CheckCollisions(float fElapsedTime);

    /**
    *   
    * Simple function used to determine the coordinates of the level which are rendered. Used mainly to only render mobs and projectiles visiable. 
    */
    void GetScreenWorldBounds();
    
    /**
    *
    * Takes the screen positions sx and sy, and changes the values wx and wy to represent the place in the level which they corespond to.
    * Typically not used, in game loop but useful for debugging and troubleshooting using the mouse position
    * Used because the level is larger than the screen size
    * 
    * @param sx x coordinate on the screen 
    * @param sy y coordinate on the screen 
    * @param wx x coordinate in the level determined by sx
    * @param wy y coordinate in the level determined by sy
    * 
    */
    void ScreenToWorld(float sx, float sy, float& wx, float& wy);

    /**
    *
    * Takes the level position and changes sx and sy to the corresponding screen position. Most useful when rendering entities. 
    * Used because the level is larger than the screen size
    * 
    * @param wx x coordinate in the level 
    * @param xy y coordinate in the level 
    * @param sx x coordinate on the screen determined by wx
    * @param sy y coordinate on the screen determined by wy
    * 
    */
    void WorldToScreen(float wx, float wy, float& sx, float& sy);

    /**
    * 
    * First compresses the the projList std::vector removing all projectiles that are 'dead' / had a collision.
    * Then updates all the projectiles by the fElapsedTime using the function Dynamic::update
    * 
    * @ param fElapsedTime a float, used to advance the projectiles by the wall time since the last OnUserUpdate call.
    */
    void updateProjectiles(float fElapsedTime);

    /**
    *
    * First compresses the mobList std::vector removing all projectiles that are 'dead'.
    * Then sorts the mobList by how close the mobs are to the player (useful when moving mobs and handeling mob vs mob collisions)
    * 
    * If a mob will be render a velocity will be set to move in the direction of player if possible. Then mobList will on be updated/advance if 500 ms have passed.
    * Also note that mobs are set to move in half chunk increments. 
    * 
    * @param fElapsedTime a float, used to advance the mobs by the wall time since the last OnUserUpdate call.
    * 
    */
    void updateMobs(float fElapsedTime);
    
    /**
    * 
    * Finds the spawner Tiles and checks to see if the player is within a certain distance ~10 tiles. If within the threshold the spawner will look to spawn
    * its particular mob type (ghost or demon) on one of the available adjancent floor tiles surrounding the spawner.
    * 
    * @param fElapsedTime a float, used to determine spawnrate by the wall time since the last OnUserUpdate call.
    * 
    */
    void updateTiles(float fElapsedTime);

    /**
    * 
    * Uses the gameObjects array to render all the objects inheriting only from Tile, Static, or InteractionBlock classes: wall, floor, key, consumable, spawner
    *
    */
    void DrawLevel();

    /**
    * Renders the game info: current level, Hp/timer, available keys, and potions
    */
    void DrawInfo();

    /**
    * Renders the Player, projectiles, and mobs.
    */
    void DrawEntities();

    /**
    * 
    * Basic IO for loading the chosen level data into the gameObjects member variable, and starting position for Mobs
    * 
    * @param index a integer, used to choose what level will be loaded 
    * 
    * @return true if no error was found, false if could not open
    * 
    */
    bool loadLevel(int index);

    ///@}

};

#endif

