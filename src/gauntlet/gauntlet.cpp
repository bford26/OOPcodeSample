#include "gauntlet.h"

Gauntlet::Gauntlet(){

    // sets window title 
    sAppName = "Gauntlet Demo";

}

bool Gauntlet::OnUserCreate(){

    // Here we load all the data for the start of the game
    // Also notice no creation of a Reasources object... it is a singlton! and we just use the static method get() for access to it

    Resources::get().loadSprites();

    decalList = new olc::Decal* [5];

    decalList[0] = new olc::Decal(Resources::get().getSprite("lvlSheet"));
    decalList[1] = new olc::Decal(Resources::get().getSprite("player"));
    decalList[2] = new olc::Decal(Resources::get().getSprite("ghost"));
    decalList[3] = new olc::Decal(Resources::get().getSprite("demon"));
    decalList[4] = new olc::Decal(Resources::get().getSprite("p1Proj"));

    infoDecal1 = new olc::Decal(Resources::get().getSprite("info1"));
    infoDecal2 = new olc::Decal(Resources::get().getSprite("info2"));

    p1 = new Player();
    p1->setSpeed(100);
    p1->setHp(500);
    
    /*
    Typically a game menu would be used to choose the desired level, but for a simple demo game states are not required, the game just starts into a level automatically so the level can be hardcoded.
    */ 
    nLevel = 1;
    loadLevel(nLevel);

    ExitGame = -1;

    lastTime = 0.0f;
    curTime = 0.0f;

    return true;
}

bool Gauntlet::OnUserUpdate(float fElapsedTime){

    curTime = fElapsedTime + curTime;


    if(curTime - lastTime >= 2.0f && p1->getVelocity().mag() > 0.0f){
        frameBool = !frameBool;
    }

    // this will update the player based on the input keys W,A,S,D 
    updatePlayer(fElapsedTime);
    updateMobs(fElapsedTime);
    updateProjectiles(fElapsedTime);
    // updateTiles(fElapsedTime);
    // utilFunc(fElapsedTime);
    // DrawDecals();

    DrawLevel();
    DrawEntities();

    std::cout << mobList.size() << std::endl;

    if(GetKey(olc::ESCAPE).bPressed){
        bGameLoop = false;
    }

    return bGameLoop;
}

bool Gauntlet::OnUserDestroy(){

    // makeing sure to clean up all created memory for no memory leaks!

    delete p1;

    for(int i=0; i<5; i++)
        delete decalList[i];
    delete[] decalList;

    delete infoDecal1;
    delete infoDecal2;

    for(int i=0; i<vertTiles*horTiles; i++)
        delete gameObjects[i];
    delete[] gameObjects;

    return true;
}   

void Gauntlet::DrawDecals(){

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

                if(value >= 0 && value <= 18)
                    gameObjects[i*horTiles+j] = new Wall(value);

                else if(value == 20)
                    gameObjects[i*horTiles+j] = new Key(value);

                else if(value == 21 || value == 22)
                    gameObjects[i*horTiles+j] = new Door(value);

                else if(value >= 23 && value <= 25)
                    gameObjects[i*horTiles+j] = new Consumable(value);

                else if(value >= 26 && value <= 31 )
                    gameObjects[i*horTiles+j] = new Spawner(value);

                else if(value == 99){
                    gameObjects[i*horTiles+j] = new Floor();
                    // set player start position
                    p1->setPosition( (float)j*16, (float)i*16 );

                }
                else if(value == 35){
                    gameObjects[i*horTiles+j] = new Floor();
                    // create ghost at this posistion
                    mobList.push_back(Mob(value, (float)j*16, (float)i*16));

                }
                else if(value == 36){
                    gameObjects[i*horTiles+j] = new Floor();
                    // create demon at this posistion
                    mobList.push_back(Mob(value, (float)j*16, (float)i*16));

                }
                else if(value == 32 || value == 33){
                    gameObjects[i*horTiles+j] = new Exit(value);

                }else{
                    gameObjects[i*horTiles+j] = new Floor();
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

    // static olc::vf2d oldVel;

    // Here we get the direction the play wants to move
    if(GetKey(olc::S).bHeld || GetKey(olc::S).bPressed){
        vel += {0.0f, 1.0f};        
    }
    
    if(GetKey(olc::W).bHeld || GetKey(olc::W).bPressed){
        vel += {0.0f , -1.0f};
    }
    
    if(GetKey(olc::A).bHeld || GetKey(olc::A).bPressed){
        vel += {-1.0f , 0.0f};
    }
    
    if(GetKey(olc::D).bHeld || GetKey(olc::D).bPressed){
        vel += {1.0f , 0.0f};
    }


    // if(p1->getVelocity().mag() != 0.0f)
    //     oldVel = p1->getVelocity().norm();

    vel *= speed;
    p1->setVelocity(vel.x, vel.y);
    vel = p1->getVelocity();

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

    left_bound  = int( pPos.x / 16) - 2;
    right_bound = int( pPos.x / 16) + 2;
    top_bound   = int( pPos.y / 16) - 2;
    bot_bound   = int( pPos.y / 16) + 2;

    if(left_bound < 0) left_bound = 0;
    if(right_bound > horTiles) right_bound = horTiles; 
    if(top_bound < 0) top_bound = 0;
    if(bot_bound > vertTiles) bot_bound = vertTiles;

    for(int i=left_bound; i<right_bound; i++)
        for(int j=top_bound; j<bot_bound; j++){

            if(DynamicVsTile(p1, fElapsedTime, *gameObjects[j*horTiles+i], cp, cn, t))
            {
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

    
    // set the new direction using the new velocity
    direction d = p1->getVelocityDirection();
    p1->setDir(d);


     // player vs mobs

    for(auto &m : mobList)
    {

        olc::vf2d mpos = m.getPosition();

        if( mpos.mag() - p1->getPosition().mag() < 16*3 )
        {
            ResolveCollision(p1, fElapsedTime, &m);
        }
    }


    // update player pos
    pPos = pPos + p1->getVelocity() * fElapsedTime;
    p1->setPosition(pPos.x, pPos.y);


    if( GetKey(olc::SPACE).bPressed ){

        Proj p(37);

        p.dead = false;
        p.setDir(p1->getDir());

        vel = p1->getDirectionVector();

        p.setPosition( p1->getPosition().x + 8*vel.x , p1->getPosition().y + 8*vel.y );

        p.setVelocity( vel.x * p.getSpeed() , vel.y * p.getSpeed());

        projList.push_back(p);

    }

}

void Gauntlet::updateMobs(float fElapsedTime){

    // check if mobs can see player..
    // if so move mobs towards player


    // int aliveCount=0;
    // for(auto &m : mobList)
    // {
    //     if(!m.dead)
    //         aliveCount++;
    // }

    // std::vector<Mob> tempVec(aliveCount);
    
    // int count = 0;
    // for(int i=0; i<mobList.size(); i++)
    // {
    //     if(!mobList[i].dead)
    //     {
    //         tempVec[count] = mobList[i];
    //         count++;
    //     }
    // }

    // mobList.clear();
    // mobList = tempVec;
    // tempVec.clear();
    int mcount =0;
    for( auto& m : mobList){

        if(m.dead == false)
        {

            olc::vf2d pos = m.getPosition(), vel;

            // if player is in a range of mob , it will want to move to them
            if( (pos - p1->getPosition()).mag() <= 16*8)
            {
                // get the move direction from player 
                vel = (p1->getPosition() - pos).norm();
                vel *= m.getSpeed(); 

            }else{

                vel = {0.0f, 0.0f};

            } 

            m.setVelocity(vel.x , vel.y);

            // collision detection here 
            olc::vf2d cp, cn;
            float t=0, min_t = INFINITY;
            std::vector<std::pair<int,float>> z;
            
            int left_bound, right_bound;
            int top_bound, bot_bound;

            left_bound  = int(pos.x /16) - 2;
            right_bound = int(pos.x /16) + 2;
            top_bound   = int(pos.y /16) - 2;
            bot_bound   = int(pos.y /16) + 2;

            if(left_bound < 0) left_bound = 0;
            if(right_bound > horTiles) right_bound = horTiles; 
            if(top_bound < 0) top_bound = 0;
            if(bot_bound > vertTiles) bot_bound = vertTiles;
            
            for(int i=left_bound; i<right_bound; i++)
                for(int j=top_bound; j<bot_bound; j++){

                    if(DynamicVsTile(&m, fElapsedTime, *gameObjects[j*horTiles+i], cp, cn, t))
                    {
                        z.push_back({j*horTiles+i,t});
                    }
                }

            std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
            {
                return a.second < b.second;
            });

            // resolve collisions in proper order 
            for(auto& j : z)
                ResolveCollision(&m, fElapsedTime, gameObjects[j.first], j.first);



            // mobs vs other mobs

            for(int j=0; j< mobList.size(); j++)
            {
                
                if(mcount != j)
                {
                    ResolveCollision(&m, fElapsedTime, &mobList[j]);
                }
            }
            
            m.setDir(m.getVelocityDirection());
            m.setPosition( pos + m.getVelocity() * fElapsedTime );
        }

        mcount++;
    }
}

void Gauntlet::updateProjectiles(float fElapsedTime){

    // using the facing direction, move projectile in their direction
    olc::vf2d pos, vel;

    
    int aliveCount=0;
    for(auto &p : projList)
    {
        if(!p.dead)
            aliveCount++;
    }

    // std::vector<Proj> tempVec(aliveCount);
    
    // int count = 0;
    // for(auto &p : projList)
    // {
    //     if(!p.dead)
    //     {
    //         tempVec[count] = Proj(p);
    //         count++;
    //     }
    // }

    // projList.clear();
    // projList = tempVec;
    // tempVec.clear();

    for( auto &p : projList )
    {            

        pos = p.getPosition();
        vel = p.getVelocity();
        
        // collision detection here 
        olc::vf2d cp, cn;
        float t=0, min_t = INFINITY;
        std::vector<std::pair<int,float>> z;
        
        int left_bound, right_bound;
        int top_bound, bot_bound;

        left_bound  = int(pos.x /16) - 2;
        right_bound = int(pos.x /16) + 2;
        top_bound   = int(pos.y /16) - 2;
        bot_bound   = int(pos.y /16) + 2;

        if(left_bound < 0) left_bound = 0;
        if(right_bound > horTiles) right_bound = horTiles; 
        if(top_bound < 0) top_bound = 0;
        if(bot_bound > vertTiles) bot_bound = vertTiles;
        
        for(int i=left_bound; i<right_bound; i++)
            for(int j=top_bound; j<bot_bound; j++){

                if(DynamicVsTile(&p, fElapsedTime, *gameObjects[j*horTiles+i], cp, cn, t))
                {
                    z.push_back({j*horTiles+i,t});
                }
            }

        std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
        {
            return a.second < b.second;
        });

        // resolve collisions in proper order 
        for(auto& j : z)
            ResolveCollision(&p, fElapsedTime, gameObjects[j.first], j.first);


        
        
        
        // projectiles vs mobs

        for(auto &m : mobList)
        {

            olc::vf2d mpos = m.getPosition();

            if( mpos.mag() - pos.mag() < 16*3 )
            {
                ResolveCollision(&p, fElapsedTime, &m);
            }

        }

        vel = p.getVelocity();
        pos = pos + vel * fElapsedTime;
        p.setPosition(pos.x, pos.y);

    }
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

    if(DynamicVsTile(pDyn, fTimeStep, *pTile, contact_point, contact_normal, contact_time))
    {

        int tileType = pTile->getType();
        
        if( pDyn->getType() == 34 )
        {
        
            if(tileType == 20)
            {
                // key
                p1->keyValue++;
                gameObjects[index]->setType(19);

            }else if(tileType >= 21 && tileType <= 22 && p1->keyValue > 0){

                // Door
                p1->keyValue--;
                if(tileType == 22)
                {
                    // clear horz door
                    for(int i=-3; i<=3; i++){
                        if(index+i < vertTiles*horTiles && index+i*horTiles >= 0)
                            if(gameObjects[index+i]->getType() == 22){
                                gameObjects[index+i]->setType(19);
                            }
                    }

                }else{
                    // clear vert door
                    for(int i=-3; i<=3; i++){
                        if(index+i*horTiles < vertTiles*horTiles && index+i*horTiles >= 0)
                            if(gameObjects[index+i*horTiles]->getType() == 21){
                                gameObjects[index+i*horTiles]->setType(19);
                            }
                    }
                }

            }else if(tileType >= 23 && tileType <= 25){
                // consumable
                if(tileType == 23)
                    p1->treasure + 100;
                else if(tileType == 24)
                    p1->potions++;
                else
                    p1->setHp(p1->getHp() + 100);

                gameObjects[index]->setType(19);

            }else if(tileType >= 26 && tileType <= 31){
                // spawners
                p1->setHp( p1->getHp() - (tileType - 25)*50 );
                gameObjects[index]->setType(19);

            }else if(tileType >= 32 && tileType <= 33){
                // exit
                bGameLoop = false;

            }else if (tileType != 19){
                // solidTile
                olc::vf2d dynVel = pDyn->getVelocity();
                dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
                pDyn->setVelocity(dynVel.x, dynVel.y);
            }

       }else if( pDyn->getType() == 35 || pDyn->getType() == 36 ){

           if(  !(tileType >= 23 && tileType <= 25) && !(tileType >= 19 && tileType <= 20)   )
           {
               // solidTile
               olc::vf2d dynVel = pDyn->getVelocity();
               dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
               pDyn->setVelocity(dynVel.x, dynVel.y);
           }

       }else if( pDyn->getType() >= 37 && pDyn->getType() <= 40 ){

           if(tileType >= 0 && tileType <= 18)
           {
               pDyn->dead = true;
               // solidTile
               olc::vf2d dynVel = pDyn->getVelocity();
               dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
               pDyn->setVelocity(dynVel.x, dynVel.y);

           }else if(tileType >= 26 && tileType <= 31){
                
                pDyn->dead = true;

                if(tileType == 26 || tileType == 29)
                    gameObjects[index]->setType(19);
                else
                    gameObjects[index]->setType(tileType-1);
               
           }



       }

    //    if(tileType >= 0 && tileType <= 18)
    //    if(tileType == 19)
    //    if(tileType == 20)
    //    if(tileType >= 21 && tileType <= 22)
    //    if(tileType >= 23 && tileType <= 25)
    //    if(tileType >= 26 && tileType <= 31)
    //    if(tileType >= 32 && tileType <= 33)

    //    if(tileType >= 35 && tileType <= 36)
    //    if(tileType >= 37 && tileType <= 40)

        return true;
    }

    return false;
}

// player vs mob  or mob vs mob or proj vs mob
bool Gauntlet::ResolveCollision(Dynamic* pDyn, const float fTimeStep, Mob *m){

    olc::vf2d contact_point, contact_normal;
    float contact_time = 0.0f;

    if (DynamicVsTile(pDyn, fTimeStep, *m, contact_point, contact_normal, contact_time) && m->dead == false)
    {
        if( pDyn->getType() == 34 )
        {
            if( m->getType() == 35 || m->getType() == 36 )
            {
                p1->setHp(p1->getHp() - 50);
                m->setHp(m->getHp() - 50);

            }
        }
        else if( pDyn->getType() == 35 || pDyn->getType() == 36 )
        {
            // acts like solidTile
            olc::vf2d dynVel = pDyn->getVelocity();
            dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
            pDyn->setVelocity(dynVel.x, dynVel.y);

        }
        else if(pDyn->getType() >= 37 && pDyn->getType() <= 39 && pDyn->dead == false)
        {
            m->setHp(m->getHp() - 100);
            pDyn->dead = true;
        }

        if(m->getHp() <= 0)
            m->dead = true;

        return true;

    }

    return false;
}

// dynamic vs proj
bool Gauntlet::ResolveCollision(Dynamic* pDyn, const float fTimeStep, Proj *p){

    olc::vf2d contact_point, contact_normal;
    float contact_time = 0.0f;

    if (DynamicVsTile(pDyn, fTimeStep, *p, contact_point, contact_normal, contact_time) && p->dead == false)
    {
        if( pDyn->getType() == 34 )
        {
            if( p->getType() == 40 )
            {
                p1->setHp(p1->getHp() - 50);
                p->dead = true;
            }
        }

        return true;
    }

    return false;
}

void Gauntlet::DrawLevel(){

    // calculations for the camera to follow the player
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

    // SetPixelMode(olc::Pixel::ALPHA);
    // drawing visible tile map!
    for(int i=-1; i < nVisTilesX+1; i++)
        for(int j=-1; j < nVisTilesY+1; j++){

            auto tile = getTile(j+fOffsetY,i+fOffsetX);

            type = tile->getType();

            DrawPartialDecal( { (float) i*16 - fTileOffsetX , (float)j*16 - fTileOffsetY }, decalList[0], {(float) (type%8)*16, (float) (type/8)*16 }, {16,16});

        } 


    // display health, keys, and potions
    DrawDecal({16*2,16*2},infoDecal1);
    DrawStringDecal({16*2+8,16*2},std::to_string(p1->getHp()), olc::WHITE, {1.0f, 1.0f});
    DrawStringDecal({16*2+8,16*2+8},std::to_string(p1->keyValue), olc::WHITE, {1.0f, 1.0f});
    DrawStringDecal({16*2+24,16*2+8},std::to_string(p1->potions), olc::WHITE, {1.0f, 1.0f});
    
    // display world level
    char buffer [2];
    sprintf(buffer, "%02d", nLevel);

    DrawDecal({ScreenWidth()/2.0f, 24}, infoDecal2);
    DrawStringDecal({ScreenWidth()/2.0f, 24}, std::string(buffer), olc::WHITE, {1.0f, 1.0f});    

}
 
void Gauntlet::DrawEntities(){

    olc::vf2d playerPosition = p1->getPosition();

    // calculations for the camera to follow the player
    int nVisTilesX = ScreenWidth()/16;
    int nVisTilesY = ScreenHeight()/16;

    float offx = 0 , offy = 0;
    float xScreenOffset = nVisTilesX / 2.0f;
    float yScreenOffset = nVisTilesY / 2.0f;

    if(playerPosition.x < xScreenOffset*16)
        offx = xScreenOffset*16-playerPosition.x;
    else if(playerPosition.x > (horTiles - xScreenOffset)*16)
        offx = (horTiles - xScreenOffset)*16 - playerPosition.x;

    if(playerPosition.y < yScreenOffset*16)
        offy = yScreenOffset*16-playerPosition.y;
    else if(playerPosition.y > (vertTiles - yScreenOffset)*16)
        offy = (vertTiles - yScreenOffset)*16 - playerPosition.y;

    // Draw Player
    DrawPartialDecal( { (float)(ScreenWidth()/2.0f - offx) , (float)(ScreenHeight()/2.0f - offy) }, decalList[1], { (float) p1->getDir()*16, 16*frameBool}, {16, 16} );


    // Draw mobs
    int x0,y0,x1,y1;
    float fOffsetX, fOffsetY;

    float fCamPosX = playerPosition.x/16;
    float fCamPosY = playerPosition.y/16;
    
    fOffsetX = fCamPosX - (float) nVisTilesX / 2.0f;
    fOffsetY = fCamPosY - (float) nVisTilesY / 2.0f;

    if(fOffsetX < 0) fOffsetX = 0;
    if(fOffsetY < 0) fOffsetY = 0;
    if(fOffsetX > horTiles  - nVisTilesX) fOffsetX = horTiles  - nVisTilesX;
    if(fOffsetY > vertTiles - nVisTilesY) fOffsetY = vertTiles - nVisTilesY;
    
    x0  = fOffsetX;
    y0  = fOffsetY;
    x1 = fOffsetX + nVisTilesX;
    y1 = fOffsetY + nVisTilesY;

    // draw mobs and projectiles
    for(auto &m : mobList){

        if(m.dead == false)
        {
            olc::vf2d mobPos = m.getPosition();

            float moffx = playerPosition.x - mobPos.x;
            float moffy = playerPosition.y - mobPos.y;

            float mdx = ScreenWidth() / 2.0f - offx - moffx;
            float mdy = ScreenHeight()/ 2.0f - offy - moffy;

            if(mobPos.x >= x0*16 && mobPos.x < x1*16)
                if(mobPos.y >= y0*16 && mobPos.y < y1*16){

                    // DrawDecal( {  (float) mdx , (float) mdy } , decalList[2]);
                    if(m.getType() == 35)
                        DrawPartialDecal( { (float) mdx , (float) mdy } , decalList[ 2 ] , { (float) m.getDir()*16, 16*frameBool}, {16, 16} );     
                    else if(m.getType() == 36)
                        DrawPartialDecal( { (float) mdx , (float) mdy } , decalList[ 3 ] , { (float) m.getDir()*16, 16*frameBool}, {16, 16} );

                }
        }
    }



    for(auto &p : projList){

        if(p.dead == false)
        {

            olc::vf2d pPos = p.getPosition();

            float poffx = playerPosition.x - pPos.x;
            float poffy = playerPosition.y - pPos.y;

            float pdx = ScreenWidth() / 2.0f - offx - poffx;
            float pdy = ScreenHeight()/ 2.0f - offy - poffy;

            if(pPos.x >= x0*16 && pPos.x < x1*16)
                if(pPos.y >= y0*16 && pPos.y < y1*16){

                    DrawPartialDecal( { (float) pdx , (float) pdy }, decalList[4], { (float) p.getDir()*16,0}, {16, 16} );

                }

        }
    }
}

void Gauntlet::updateTiles(float fElapsedTime){

    for(int i=0; i < vertTiles*horTiles; i++)
    {

        int tileType = gameObjects[i]->getType();
        if(tileType >= 26 && tileType <= 31)
        {

            int mType = 0;
            if(tileType >= 26 && tileType <= 28)
                mType = 35;
            else if(tileType >= 29 && tileType <= 31)
                mType = 36;

            // make new mob in a postion 1 tile away from current tile position

            if(curTime - lastTime >= 4.0)
            {  

                lastTime = curTime;
                curTime = 0.0f;
                
                // get spawner pos
                olc::vf2d mpos = gameObjects[i]->getPosition();
                int x = mpos.x, y = mpos.y;

                // check if a mob is to any of the sides of the spawner if not make that place the posiition to spawn mob

                int nidx, eidx, widx, sidx;

                nidx = i + -1*horTiles + 0;
                eidx = i + 0*horTiles + 1;
                sidx = i + 1*horTiles + 0;
                widx = i + 0*horTiles + -1;
                
                auto tileIsFloor = [&](int index){
                    
                    bool noMobs = true;

                    if(19 == gameObjects[index]->getType())
                    {                     
                        for(auto &m : mobList){
                            if( TileVsTile( &m, gameObjects[index] ) ){
                                noMobs = false;
                                break;
                            }
                        }
                    }

                    return noMobs;
                };

                if(tileIsFloor(nidx))
                {
                    x = gameObjects[nidx]->getPosition().x;
                    y = gameObjects[nidx]->getPosition().y;

                }else if( tileIsFloor(eidx)){

                    x = gameObjects[eidx]->getPosition().x;
                    y = gameObjects[eidx]->getPosition().y;

                }else if( tileIsFloor(sidx)){

                    x = gameObjects[sidx]->getPosition().x;
                    y = gameObjects[sidx]->getPosition().y;

                }else if( tileIsFloor(widx)){

                    x = gameObjects[widx]->getPosition().x;
                    y = gameObjects[widx]->getPosition().y;

                }

                Mob m(mType, x, y);
                mobList.push_back(m);                
                
            }
        }   
    }
}