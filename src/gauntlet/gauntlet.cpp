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
    p1->setPosition( (float) 16*16 , (float) 15*16);
    /*
    This will load the level from the formatted txt file under "data/levels/".
    
    Typically a game menu would be used to choose the desired level, but for a simple demo game states are not required, the game just starts into a level automatically so the level can be hardcoded.
    */ 
    loadLevel(0);

    return true;
}

bool Gauntlet::OnUserUpdate(float fElapsedTime){


    // this will update the player based on the input keys W,A,S,D 
    // updatePlayer(fElapsedTime);
    // updateMobs(fElapsedTime);
    // updateProjectiles(fElapsedTime);


    // panFunc();

    // ConvertTilesToPolyMap(0,0,horTiles,vertTiles, 16.0f, horTiles);

    // utilFunc(fElapsedTime);

    olc::vf2d vel;

    if (GetKey(olc::Key::W).bHeld) vel.y = -100.0f;
    if (GetKey(olc::Key::S).bHeld) vel.y = +100.0f;
    if (GetKey(olc::Key::A).bHeld) vel.x = -100.0f;
    if (GetKey(olc::Key::D).bHeld) vel.x = +100.0f;

    p1->setVelocity(vel.x, vel.y);

    // update player pos
    olc::vf2d pPos = p1->getPosition();
    olc::vf2d pVel = p1->getVelocity();
    pPos = pPos + pVel * fElapsedTime;
    p1->setPosition(pPos.x, pPos.y);

    Clear(olc::DARK_BLUE);

    DrawRect(pPos.x, pPos.y, 16, 16, olc::WHITE);



    // DrawDecals();

    

    // std::cout << (int) p1->getPosition().x << " , " << (int) p1->getPosition().y << std::endl;


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
    int size = gameObjects[0]->getSize();
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

            DrawDecal( { (float) i*size - fTileOffsetX , (float)j*size - fTileOffsetY } , decalList[type]);

        }


    olc::vf2d offset;
    float offx = 0 , offy = 0;


    int VisTilesX = ScreenWidth() / size;
    int VisTilesY = ScreenHeight() / size;

    float xScreenOffset = VisTilesX / 2.0f;
    float yScreenOffset = VisTilesY / 2.0f;


    if(playerPosition.x < xScreenOffset*size)
        offx = xScreenOffset*size-playerPosition.x;
    else if(playerPosition.x > (horTiles - xScreenOffset)*size)
        offx = (horTiles - xScreenOffset)*size - playerPosition.x;
    else
        offx = 0;

    if(playerPosition.y < yScreenOffset*size)
        offy = yScreenOffset*size-playerPosition.y;
    else if(playerPosition.y > (vertTiles - yScreenOffset)*size)
        offy = (vertTiles - yScreenOffset)*size - playerPosition.y;
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
    float playerSize = 16;
    olc::vf2d pPos = p1->getPosition();
    olc::vf2d moveDir = { 0.0 , 0.0 };
    direction pDir = p1->getDir();


    // Here we get the direction the play wants to move
    if(GetKey(olc::S).bHeld || GetKey(olc::S).bPressed){
        
        moveDir = {0,1};
        p1->setDir(direction::SOUTH);
    }
    else if(GetKey(olc::W).bHeld || GetKey(olc::W).bPressed){
        
        moveDir = {0,-1};
        p1->setDir(direction::NORTH);    
    }
    else if(GetKey(olc::A).bHeld || GetKey(olc::A).bPressed){
        
        moveDir = {-1,0};
        p1->setDir(direction::WEST);
    }
    else if(GetKey(olc::D).bHeld || GetKey(olc::D).bPressed){
        
        moveDir = {1,0};
        p1->setDir(direction::EAST);
    }


    olc::vf2d nextPos = pPos + moveDir * speed * fElapsedTime;

    auto DoesCollide = [&](){

        bool bVar = true;

        int x0 = nextPos.x/16;
        int y0 = nextPos.y/16;

        int nidx = (y0-1)*horTiles+x0;
        int eidx = y0*horTiles+x0+1;
        int sidx = (y0+1)*horTiles+x0;
        int widx = y0*horTiles+x0-1;

        int idxArr[4] = { nidx, eidx, sidx, widx };
        
        for( auto idx : idxArr ){

            bool bNotSolidTile = gameObjects[idx]->getSolidTile();
            olc::vf2d tilePos = gameObjects[idx]->getPosition();
            

            if(!bNotSolidTile){
                                
                // this is a solid tile need to check for collision!
                if( tilePos.x < nextPos.x || nextPos.x < tilePos.x || tilePos.y < nextPos.y || nextPos.y < tilePos.y + 15.0)
                    // overlap
                    return true;

            }
            
        }

        return false;

    };


    bool anyCollide = false;

    anyCollide = DoesCollide();

    if(!anyCollide)
        p1->setPosition(nextPos.x, nextPos.y);







    // olc::vf2d nextPos = p1->getPosition() + playerDir * speed * fElapsedTime;
    // auto DoesCollide = [&](const olc::vf2d& point){
    //     olc::vi2d testPoint = nextPos/16 + point;
    //     // if(point.x == -1 && point.y == 0)
    //     //     testPoint.x += 1;
    //     // if(point.x == 0 && point.y == -1)
    //     //     testPoint.y += 1;        
    //     auto& tile = gameObjects[testPoint.y*horTiles+testPoint.x];
    //     if(tile->getEntityVsTile()){
    //         // entityvstile == true, which means it is a fine interaction
    //         return false;
    //     }else
    //         return true;
    // };
    // // here we check if the movement in the input direction would create a collision
    // bool anyCollision = false;
    // anyCollision |= DoesCollide({0,-1});
    // anyCollision |= DoesCollide({0,1});
    // anyCollision |= DoesCollide({-1,0});
    // anyCollision |= DoesCollide({1,0});
    // if(!anyCollision){
    //     p1->setPosition(nextPos.x, nextPos.y);
    // }

}

void Gauntlet::updateMobs(float fElapsedTime){

    // check if mobs can see player..
    // if so move mobs towards player

}

void Gauntlet::updateProjectiles(float fElapsedTime){

    // using the facing direction, move projectile in their direction

}

void Gauntlet::utilFunc(float fElapsedTime){

    float speed = p1->getSpeed();
    float playerSize = 16;
    olc::vf2d pPos = p1->getPosition();
    olc::vf2d moveDir = { 0.0 , 0.0 };
    direction pDir = p1->getDir();


    // Here we get the direction the play wants to move
    if(GetKey(olc::S).bHeld || GetKey(olc::S).bPressed){
        
        moveDir += {0,1};
        p1->setDir(direction::SOUTH);
    }
    else if(GetKey(olc::W).bHeld || GetKey(olc::W).bPressed){
        
        moveDir += {0,-1};
        p1->setDir(direction::NORTH);    
    }
    

    if(GetKey(olc::A).bHeld || GetKey(olc::A).bPressed){
        
        moveDir += {-1,0};
        p1->setDir(direction::WEST);
    }
    else if(GetKey(olc::D).bHeld || GetKey(olc::D).bPressed){
        
        moveDir += {1,0};
        p1->setDir(direction::EAST);
    }


    
    olc::vf2d nextPos = pPos + moveDir * speed * fElapsedTime;

    // nextPo

    olc::vf2d v1, v2, testPos;

    testPos = { (float) nextPos.x + 15.0f/2.0f , (float) nextPos.y + 15.0f/2.0f };

    auto DoesCollide = [&](){

        // here we basically go thru each edge
        // then calculate a vector, v1, for
        for(sEdge &e : vecEdges){

            // v1 is the vector from player position to the edge
            v1 = e.start - testPos;
            v2 = e.end - testPos;
            
            if( v1.mag() < playerSize * 0.9 || v2.mag() < playerSize * 0.9){
                return true;
            }
            
        }

        return false;
    };


    bool anyCollide = false;

    anyCollide = DoesCollide();

    if(!anyCollide)
        p1->setPosition(nextPos.x, nextPos.y);


}

void Gauntlet::panFunc(){


    if (GetKey(olc::Key::A).bHeld) p1->setVelocity( -1.0f * p1->getSpeed() , p1->getVelocity().y);
    if (GetKey(olc::Key::D).bHeld) p1->setVelocity( p1->getSpeed() , p1->getVelocity().y);
    if (GetKey(olc::Key::W).bHeld) p1->setVelocity( p1->getVelocity().x , -1.0f * p1->getSpeed());
    if (GetKey(olc::Key::S).bHeld) p1->setVelocity( p1->getVelocity().x , p1->getSpeed());


    olc::vf2d cp, cn;
    float t=0, min_t = INFINITY;
    std::vector<std::pair<int, float>> z;

    for(size_t i = 1; i < 10; i++ ){



    }




}

void Gauntlet::ConvertTilesToPolyMap(int sx, int sy, int w, int h, float fBlockWidth, int pitch){
    
    // sx and sy make it so that we dont have to redo the entire map just redo part we want
    // clear out the map incase any changes made to map.. basically doors removed and hidden exit
    vecEdges.clear();

    for(int x=0; x<w; x++)
        for(int y=0; y<h; y++)
            for(int j=0; j<4; j++){

                gameObjects[(y+sy)*pitch + (x+sx)]->edge_exist[j] = false;
                gameObjects[(y+sy)*pitch + (x+sx)]->edge_exist[j] = 0;

            }



    // now make the polygons
    for(int x=1; x<w-1; x++)
        for(int y=1; y<h-1; y++){

            int i = (y+sy)*pitch+(x+sx);    //current 
            int n = (y+sy-1)*pitch+(x+sx);  //north
            int e = (y+sy)*pitch+(x+sx-1);  //east
            int s = (y+sy+1)*pitch+(x+sx);  //south
            int w = (y+sy)*pitch+(x+sx+1);  //west

            //is it a solid tile? if yes it needs edge
            if(gameObjects[i]->getSolidTile()){

                // if no western block it will need west edge
                if(!gameObjects[w]->getSolidTile()){

                    //created or extended from northen edge
                    if(gameObjects[n]->edge_exist[eWEST]){

                        //north has edge so just extend it..
                        vecEdges[gameObjects[n]->edge_id[eWEST]].end.y += fBlockWidth;
                        gameObjects[i]->edge_id[eWEST] = gameObjects[n]->edge_id[eWEST];
                        gameObjects[i]->edge_exist[eWEST] = true;

                    }else{

                        sEdge edge;
                        edge.start.x = (sx+x) *fBlockWidth; edge.start.y = (sy + y)*fBlockWidth;
                        edge.end.x = edge.start.x; edge.end.y = edge.start.y + fBlockWidth;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        gameObjects[i]->edge_id[eWEST] = edge_id;
                        gameObjects[i]->edge_exist[eWEST] = true;


                    }

                }

                
                // if no eastern block
                if(!gameObjects[e]->getSolidTile()){
                    
                    //created or extended from northen edge
                    if(gameObjects[n]->edge_exist[eEAST]){

                        //north has edge so just extend it..
                        vecEdges[gameObjects[n]->edge_id[eEAST]].end.y += fBlockWidth;
                        gameObjects[i]->edge_id[eEAST] = gameObjects[n]->edge_id[eEAST];
                        gameObjects[i]->edge_exist[eEAST] = true;

                    }else{

                        sEdge edge;
                        edge.start.x = (sx+x+1) *fBlockWidth; edge.start.y = (sy + y)*fBlockWidth;
                        edge.end.x = edge.start.x; edge.end.y = edge.start.y + fBlockWidth;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        gameObjects[i]->edge_id[eEAST] = edge_id;
                        gameObjects[i]->edge_exist[eEAST] = true;


                    }
                }


                // if no nothern block
                if(!gameObjects[n]->getSolidTile()){


                    if(gameObjects[w]->edge_exist[eNORTH]){

                        vecEdges[gameObjects[w]->edge_id[eNORTH]].end.x += fBlockWidth;
                        gameObjects[i]->edge_id[eNORTH] = gameObjects[w]->edge_id[eNORTH];
                        gameObjects[i]->edge_exist[eNORTH] = true;

                    }else{

                        sEdge edge;
                        edge.start.x = (sx + x)*fBlockWidth; edge.start.y = (sy + y)*fBlockWidth;
                        edge.end.x = edge.start.x + fBlockWidth; edge.end.y = edge.start.y;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        gameObjects[i]->edge_id[eNORTH] = edge_id;
                        gameObjects[i]->edge_exist[eNORTH] = true;

                    }
                }


                // if no southern block
                if(!gameObjects[s]->getSolidTile()){

                    if(gameObjects[w]->edge_exist[eSOUTH]){

                        vecEdges[gameObjects[w]->edge_id[eSOUTH]].end.x += fBlockWidth;
                        gameObjects[i]->edge_id[eSOUTH] = gameObjects[w]->edge_id[eSOUTH];
                        gameObjects[i]->edge_exist[eSOUTH] = true;

                    }else{

                        sEdge edge;
                        edge.start.x = (sx + x) *fBlockWidth; edge.start.y = (sy + y + 1)*fBlockWidth;
                        edge.end.x = edge.start.x + fBlockWidth; edge.end.y = edge.start.y;

                        int edge_id = vecEdges.size();
                        vecEdges.push_back(edge);

                        gameObjects[i]->edge_id[eSOUTH] = edge_id;
                        gameObjects[i]->edge_exist[eSOUTH] = true;

                    }
                }


            }

        }
}


// bool Gauntlet::PointVsTile(const olc::vf2d& p, const Tile* pTile){

//     return (p.x >= pTile->pos.x && p.y >= pTile->pos.y && p.x < pTile->pos.x + pTile->size.x && p.y < pTile->pos.y + pTile->size.y);
// }


// bool Gauntlet::TileVsTile(const Tile* pt1, const Tile* pt2){

//     return (pt1->pos.x < pt2->pos.x + pt2->size.x && pt1->pos.x + pt1->size.x > pt2->pos.x && pt1->pos.y < pt2->pos.y + pt2->size.y && pt1->pos.y + pt1->size.y > pt2->pos.y);

// }

// bool RayVsTile(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const Tile* target, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near){

//     contact_normal = {0,0};
//     contact_point = {0,0};

//     olc::vf2d invdir = 1.0f / ray_dir;

//     olc::vf2d t_near = (target->pos - ray_origin) * invdir;
//     olc::vf2d t_far = (target->pos + target->size - ray_origin) * invdir;

//     if(std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
//     if(std::isnan(t_near.y) || std::isnan(t_near.x)) return false;


//     if(t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
//     if(t_near.y > t_far.y) std::swap(t_near.y, t_far.y);


//     if(t_near.x > t_far.y || t_near.y > t_far.x) return false;

    
//     t_hit_near = std::max(t_near.x, t_near.y);

//     float t_hit_far = std::min(t_far.x, t_far.y);


//     if(t_hit_far < 0) return false;

//     contact_point = ray_origin + t_hit_far * ray_dir;

//     if(t_near.x > t_near.y)
//         if(invdir.x < 0)
//             contact_normal = {1, 0};
//         else
//             contact_normal = {-1, 0};
//     else if( t_near.x < t_near.y)
//         if(invdir.y < 0)
//             contact_normal = {0 , 1};
//         else
//             contact_normal = {0, -1};


//     return true;     

// }


// bool Gauntlet::DynamicVsTile(const Dynamic* pDyn, const float fTimeStep, const Tile& pTile, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time){

//     olc::vf2d dynVel = pDyn->getVelocity();
    
//     if(dynVel.x == 0 && dynVel.y == 0)
//         return false;

//     Tile expanded_target;

//     expanded_target.setPosition(pTile.getPosition() - pDyn->getSize() / 2);
//     expanded_target.setSize(pTile.getSize() + pDyn->getSize());

//     if(RayVsTile(pDyn->getPosition() + pDyn->getSize()/2 , dynVel * fTimeStep, &expanded_target, contact_point, contact_normal, contact_time))
//         return (contact_time >= 0.0f && contact_time <  1.0f);
//     else
//         return false;
// }


// bool Gauntlet::ResolveCollision(Dynamic* pDyn, const float fTimeStep, Tile* pTile){


//     olc::vf2d contact_point, contact_normal;
//     float contact_time = 0.0f;

//     if (DynamicVsTile(pDyn, fTimeStep, *pTile, contact_point, contact_normal, contact_time)) {

//         if(contact_normal.y > 0) pDyn->contact[0] = pTile; else nullptr;
//         if(contact_normal.x < 0) pDyn->contact[1] = pTile; else nullptr;
//         if(contact_normal.y < 0) pDyn->contact[2] = pTile; else nullptr;
//         if(contact_normal.x > 0) pDyn->contact[3] = pTile; else nullptr;

//         olc::vf2d dynVel = pDyn->getVelocity();
//         dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
//         pDyn->setVelocity(dynVel.x, dynVel.y);

//         return true;
//     }

//     return false;
// }




