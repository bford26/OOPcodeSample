#include "gauntlet.h"

Gauntlet::Gauntlet(){

    // sets window title 
    sAppName = "Gauntlet Demo";

}

bool Gauntlet::OnUserCreate(){

    // Here we load all the data for the start of the game
    // Also notice no creation of a Reasources object... it is a singlton! and we just use the static method get() for access to it

    Resources::get().loadSprites();

    decalList = new olc::Decal* [decalNum];

    decalList[0] = new olc::Decal(Resources::get().getSprite("wall"));
    decalList[1] = new olc::Decal(Resources::get().getSprite("floor"));
    decalList[2] = new olc::Decal(Resources::get().getSprite("key"));
    decalList[3] = new olc::Decal(Resources::get().getSprite("door"));
    decalList[4] = new olc::Decal(Resources::get().getSprite("spawner"));
    decalList[5] = new olc::Decal(Resources::get().getSprite("treasure"));
    decalList[6] = new olc::Decal(Resources::get().getSprite("player"));
    decalList[7] = new olc::Decal(Resources::get().getSprite("ghost"));
    decalList[8] = new olc::Decal(Resources::get().getSprite("demon"));

    p1 = new Player();
    p1->setSpeed(100);
    p1->setHp(500);
    
    /*
    Typically a game menu would be used to choose the desired level, but for a simple demo game states are not required, the game just starts into a level automatically so the level can be hardcoded.
    */ 
    loadLevel(1);

    return true;
}

bool Gauntlet::OnUserUpdate(float fElapsedTime){


    // this will update the player based on the input keys W,A,S,D 
    updatePlayer(fElapsedTime);
    // updateMobs(fElapsedTime);
    // updateProjectiles(fElapsedTime);
    // utilFunc(fElapsedTime);

    DrawDecals();

    // Draw info box

    FillRect(16,16,16,32,olc::BLACK);
    DrawStringDecal({26,16},std::to_string(p1->getHp()),olc::WHITE, {1,1});
    DrawStringDecal({26,24},std::to_string(p1->keyValue), olc::WHITE, {1,1});
    DrawStringDecal({40,24},std::to_string(p1->keyValue), olc::WHITE, {1,1});

    


    // LAYERS
    // - floor
    // - walls
    // - doors
    // - mobs, projectiles

    int floorLay, dynLay;

    floorLay = CreateLayer();
    SetDrawTarget(floorLay);
    for(int i=0; i<16; i++)
        for(int j=0; j<15; j++)
            DrawDecal({i*16,j*16},decalList[1]);
    SetDrawTarget(nullptr);

    dynLay = CreateLayer();
    SetDrawTarget(dynLay);
    for(auto m : mobList){
        DrawDecal(m.getPosition(),decalList[m.getType()]);
    }

    if(GetKey(olc::ESCAPE).bPressed){
        return false;
    }

    return true;
}

bool Gauntlet::OnUserDestroy(){

    // makeing sure to clean up all created memory for no memory leaks!

    delete p1;

    for(int i=0; i<decalNum; i++)
        delete decalList[i];
    delete[] decalList;

    for(int i=0; i<vertTiles*horTiles; i++)
        delete gameObjects[i];
    delete[] gameObjects;

    return true;
}   

void Gauntlet::DrawDecals(){

    int nVisTilesX = ScreenWidth()/16;
    int nVisTilesY = ScreenHeight()/16;

    int type;
    olc::vf2d size = gameObjects[0]->getSize();
    olc::vf2d playerPosition = p1->getPosition();
    olc::vf2d drawPosition;

    float drawX, drawY;

    float fTileOffsetX, fTileOffsetY, fOffsetX, fOffsetY;


    auto getTile = [&](int i, int j){

        int idx;

        if(i*horTiles+j < horTiles*vertTiles && i*horTiles+j >= 0){

            idx = i*horTiles+j;

        }else{

            idx = 0;
        }

        return gameObjects[idx];
    };


    float fCamPosX = playerPosition.x/16;
    float fCamPosY = playerPosition.y/16;

    
    fOffsetX = fCamPosX - (float) nVisTilesX / 2.0f;
    fOffsetY = fCamPosY - (float) nVisTilesY / 2.0f;


    if(fOffsetX < 0) fOffsetX = 0;
    if(fOffsetY < 0) fOffsetY = 0;
    if(fOffsetX > horTiles  - nVisTilesX) fOffsetX = horTiles  - nVisTilesX;
    if(fOffsetY > vertTiles - nVisTilesY) fOffsetY = vertTiles - nVisTilesY;


    // so tiles move smooth!
    fTileOffsetX = (fOffsetX - (int)fOffsetX)*16;
    fTileOffsetY = (fOffsetY - (int)fOffsetY)*16;


   // drawing visible tile map!
    for(int i=-1; i < nVisTilesX+1; i++)
        for(int j=-1; j < nVisTilesY+1; j++){

            auto tile = getTile(j+fOffsetY,i+fOffsetX);

            type = tile->getType();

            if(type >= decalNum || type < 0)
                type = 0;

            DrawDecal( { (float) i*16 - fTileOffsetX , (float)j*16 - fTileOffsetY } , decalList[type]);

        }


    olc::vf2d offset;
    float offx = 0 , offy = 0;


    int VisTilesX = ScreenWidth() / 16;
    int VisTilesY = ScreenHeight() / 16;

    float xScreenOffset = VisTilesX / 2.0f;
    float yScreenOffset = VisTilesY / 2.0f;


    if(playerPosition.x < xScreenOffset*16)
        offx = xScreenOffset*16-playerPosition.x;
    else if(playerPosition.x > (horTiles - xScreenOffset)*16)
        offx = (horTiles - xScreenOffset)*16 - playerPosition.x;
    else
        offx = 0;

    if(playerPosition.y < yScreenOffset*16)
        offy = yScreenOffset*16-playerPosition.y;
    else if(playerPosition.y > (vertTiles - yScreenOffset)*16)
        offy = (vertTiles - yScreenOffset)*16 - playerPosition.y;
    else
        offy = 0;


    offset = {offx, offy};

    DrawDecal( { (float)(ScreenWidth()/2 - offset.x) , (float)(ScreenHeight()/2 - offset.y) }, decalList[6]);


}

bool Gauntlet::loadLevel(int index){

    std::string sIndex = std::to_string(index);

    switch (sIndex.length())
    {
    case 1:
        sIndex = "00" + sIndex;
        break;

    case 2:
        sIndex = "0" + sIndex;
        break;

    case 3:
        break;

    default:
        std::cout << "Error: level " << sIndex << " is not valid!\n";
        return false;
        break;
    }

    std::string fName = "./data/levels/lvl" + sIndex + ".txt";

    std::ifstream fin;
    std::string line;
    std::stringstream lineStream;
    int value;

    fin.open(fName, std::ifstream::in);

    if(fin.is_open()){
        
        // level name
        getline(fin, line);
        // height
        getline(fin, line);
        // width
        getline(fin, line);

        // getline(fin, line);
        // std::cout << line << std::endl;
        // now we are out of the commented portion with lvl info and get the level design
        //make sure we have an array to store information about the level in
        // here we are indexing into a flattened 2d array for the lvl map then setting the type of that tile object 
        // also note that we are taking advantage of polymorphism to store all the game objects
        // some objects in the gameObjects array will just be simple walls, where others will be exits, spawners, etc.. which we have control over

        horTiles = 32;
        vertTiles = 30;

        // auto DetermineTileType = [&](int index, int readVal){};

        gameObjects = new Tile* [horTiles * vertTiles];
        
        for(int i=0; i<vertTiles; i++){

            getline(fin, line);
            lineStream.str(line);

            for(int j=0; j<horTiles; j++){

                getline(lineStream, line, ',');
                value = std::stoi(line);

                if(value == 0)
                    gameObjects[i*horTiles+j] = new Wall(value);
                else if(value == 1)
                    gameObjects[i*horTiles+j] = new Floor();
                else if(value == 2)
                    gameObjects[i*horTiles+j] = new Key();
                else if(value == 3)
                    gameObjects[i*horTiles+j] = new Door(value);
                else if(value == 4)
                    gameObjects[i*horTiles+j] = new Spawner(value);
                else if(value == 5)
                    gameObjects[i*horTiles+j] = new Consumable(value);
                else if(value == 6){
                    gameObjects[i*horTiles+j] = new Floor();
                    
                    // set player start position
                    // make sure p1 is actual obj created already
                    if(p1 != nullptr)
                        p1->setPosition( (float)j*16, (float)i*16 );
                }
                else if(value == 7){
                    gameObjects[i*horTiles+j] = new Floor();
                    // create ghost at this posistion
                    mobList.push_back(Mob(0, (float)j*16, (float)i*16));
                }
                else if(value == 8){
                    gameObjects[i*horTiles+j] = new Floor();

                    // create demon at this posistion
                    mobList.push_back(Mob(1, (float)j*16, (float)i*16));
                }

                gameObjects[i*horTiles+j]->setPosition(j*16,i*16);

            }

            lineStream.clear();
        }

        // making sure to close file after opened
        fin.close();

    }else{
        std::cout << "Error: level file could not open!\n";
        return false;
    }

    return true;
}

void Gauntlet::updatePlayer(float fElapsedTime){

    float speed = p1->getSpeed();
    olc::vf2d vel = {0.0f, 0.0f};
    olc::vf2d pPos = p1->getPosition();
    direction pDir = p1->getDir();

    // Here we get the direction the play wants to move
    if(GetKey(olc::S).bHeld || GetKey(olc::S).bPressed){
        
        vel += {0.0f, 1.0f};
        
        p1->setDir(direction::SOUTH);
    }
    
    if(GetKey(olc::W).bHeld || GetKey(olc::W).bPressed){
        
        vel += {0.0f , -1.0f};
        p1->setDir(direction::NORTH);    
    }
    
    if(GetKey(olc::A).bHeld || GetKey(olc::A).bPressed){
        
        vel += {-1.0f , 0.0f};
        p1->setDir(direction::WEST);
    }
    
    if(GetKey(olc::D).bHeld || GetKey(olc::D).bPressed){
        
        vel += {1.0f , 0.0f};
        p1->setDir(direction::EAST);
    }

    vel *= speed;
    p1->setVelocity(vel.x, vel.y);

    // sort collisions by distance
    olc::vf2d cp, cn;
    float t=0, min_t = INFINITY;
    std::vector<std::pair<int,float>> z;

    // work out collisions

    /* take the player position, get a chunk of the world around the player
    then from that chunk add tile that can be interacted with into a vector list
    then for each object in the list check for collisions
    */ 
    
    int left_bound, right_bound;
    int top_bound, bot_bound;

    left_bound  = int( pPos.x / 16) - 3;
    right_bound = int( pPos.x / 16) + 3;
    top_bound   = int( pPos.y / 16) - 3;
    bot_bound   = int( pPos.y / 16) + 3;

    if(left_bound < 0) left_bound = 0;
    if(right_bound > horTiles) right_bound = horTiles; 
    if(top_bound < 0) top_bound = 0;
    if(bot_bound > vertTiles) bot_bound = vertTiles;

    for(int i=left_bound; i<right_bound; i++)
        for(int j=top_bound; j<bot_bound; j++){

            if(gameObjects[j*horTiles+i]->getType() != 1)
                if(DynamicVsTile(p1, fElapsedTime, *gameObjects[j*horTiles+i], cp, cn, t)){

                    z.push_back({j*horTiles+i,t});
                }
        }   


    // sort the list z by distance 
    std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
    {
        return a.second < b.second;
    });


    // resolve collisions in proper order 
    for(auto j : z)
        ResolveCollision(p1, fElapsedTime, gameObjects[j.first], j.first);


    // update player pos
    pPos = pPos + p1->getVelocity() * fElapsedTime;
    p1->setPosition(pPos.x, pPos.y);    

}

void Gauntlet::updateMobs(float fElapsedTime){

    // check if mobs can see player..
    // if so move mobs towards player

}

void Gauntlet::updateProjectiles(float fElapsedTime){

    // using the facing direction, move projectile in their direction
    

}

void Gauntlet::utilFunc(float fElapsedTime){

    // auto mobAI = [&](Mob &m){



    // }

    // for(auto m : mobList){

        

    // }


    // for(auto p : projList){




    // }

}

// This function will return a bool if a point provided is inside the bounds of a tile object
// pTile is pointer to the Tile
// p is the 2D float passed by reference, which holds the point location
bool Gauntlet::PointVsTile(const olc::vf2d& p, const Tile* pTile){

    olc::vf2d pos = pTile->getPosition();
    olc::vf2d size = pTile->getSize();

    return (p.x >= pos.x && p.y >= pos.y && p.x < pos.x + size.x && p.y < pos.y + size.y);
}

// TileVsTile returns true if the two Tile objects are overlapping
bool Gauntlet::TileVsTile(const Tile* pt1, const Tile* pt2){

    olc::vf2d t1Pos = pt1->getPosition(), t1Size = pt1->getSize();
    olc::vf2d t2Pos = pt2->getPosition(), t2Size = pt2->getSize();

    return (t1Pos.x < t2Pos.x + t2Size.x && t1Pos.x + t1Size.x > t2Pos.x && t1Pos.y < t2Pos.y + t2Size.y && t1Pos.y + t1Size.y > t2Pos.y);
}

// RayVsTile returns true if a ray passing through the passed target Tile
// this function also calculates the first contact_point for a ray and tile
// along with the noraml vector for the contact surface 
bool Gauntlet::RayVsTile(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const Tile* target, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near){

    contact_normal = {0,0};
    contact_point = {0,0};

    olc::vf2d invdir = 1.0f / ray_dir;
    olc::vf2d t_near, t_far, pos, size;

    pos = target->getPosition();
    size = target->getSize();

    t_near = (pos - ray_origin) * invdir;
    t_far = (pos + size - ray_origin) * invdir;

    if(std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
    if(std::isnan(t_near.y) || std::isnan(t_near.x)) return false;


    if(t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if(t_near.y > t_far.y) std::swap(t_near.y, t_far.y);


    if(t_near.x > t_far.y || t_near.y > t_far.x) return false;

    
    t_hit_near = std::max(t_near.x, t_near.y);

    float t_hit_far = std::min(t_far.x, t_far.y);


    if(t_hit_far < 0) return false;

    contact_point = ray_origin + t_hit_far * ray_dir;

    if(t_near.x > t_near.y)
        if(invdir.x < 0)
            contact_normal = {1, 0};
        else
            contact_normal = {-1, 0};
    else if( t_near.x < t_near.y)
        if(invdir.y < 0)
            contact_normal = {0 , 1};
        else
            contact_normal = {0, -1};


    return true;     

}

// DynVsTile is used for Dynamic and Tile object collisions 
// returns true if a collision will happen
bool Gauntlet::DynamicVsTile(const Dynamic* pDyn, const float fTimeStep, const Tile& pTile, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time){

    olc::vf2d dynVel = pDyn->getVelocity();
    
    if(dynVel.x == 0 && dynVel.y == 0)
        return false;

    Tile expanded_target;
    expanded_target.setPosition(pTile.getPosition() - pDyn->getSize() / 2);
    expanded_target.setSize(pTile.getSize() + pDyn->getSize());

    if(RayVsTile(pDyn->getPosition() + pDyn->getSize()/2 , dynVel * fTimeStep, &expanded_target, contact_point, contact_normal, contact_time))
        return (contact_time >= 0.0f && contact_time <  1.0f);
    else
        return false;
}

// Calling this function will make sure no collisions are allowed and that the dynamic obj will still move if collisions occur
bool Gauntlet::ResolveCollision(Dynamic* pDyn, const float fTimeStep, Tile* pTile, int index){

    olc::vf2d contact_point, contact_normal;
    float contact_time = 0.0f;

    if (DynamicVsTile(pDyn, fTimeStep, *pTile, contact_point, contact_normal, contact_time)) {

        int tileType = pTile->getType();

        switch(tileType){

            // key
            case 2:
                // add 1 point to key value
                p1->keyValue++;

                // remove key and replace with floor
                gameObjects[index]->setType(1);
                // delete gameObjects[index];
                // gameObjects[index] = new Floor();
                break;

            // door
            case 3:
                if(p1->keyValue > 0){
                    p1->keyValue--;

                    // remove door
                    gameObjects[index]->setType(1);

                    // clears horizontal doors
                    for(int i=-3; i<=3; i++){
                        if(index+i < vertTiles*horTiles && index+i*horTiles >= 0)
                            if(gameObjects[index+i]->getType() == 3){
                                gameObjects[index+i]->setType(1);
                            }
                    }

                    // clears vertial doors
                    for(int i=-3; i<=3; i++){

                        if(index+i*horTiles < vertTiles*horTiles && index+i*horTiles >= 0)
                            if(gameObjects[index+i*horTiles]->getType() == 3){
                                gameObjects[index+i*horTiles]->setType(1);
                            }
                    }


                }else{

                    olc::vf2d dynVel = pDyn->getVelocity();
                    dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
                    pDyn->setVelocity(dynVel.x, dynVel.y);
                }
                    
                break;

            // spawner
            case 4:
                // hurt player? remove spawner
                p1->setHp(p1->getHp() - 20);

                // delete gameObjects[index];
                // gameObjects[index] = new Floor();
                gameObjects[index]->setType(1);
                break;

            // treasure
            case 5:
                p1->treasure + 100;

                // delete gameObjects[index];
                // gameObjects[index] = new Floor();
                gameObjects[index]->setType(1);
                break;

            default:
                olc::vf2d dynVel = pDyn->getVelocity();
                dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
                pDyn->setVelocity(dynVel.x, dynVel.y);

        }

        return true;
    }

    return false;
}

