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

    utilDecal = new olc::Decal(Resources::get().getSprite("startScreen"));

    outlineDecal = new olc::Decal(Resources::get().getSprite("outline"));


    p1 = new Player();
    p1->setSpeed(100);
    p1->setHp(500);
    
    /*
    Typically a game menu would be used to choose the desired level, but for a simple demo game states are not required, the game just starts into a level automatically so the level can be hardcoded.
    */ 
    nLevel = 1;
    loadLevel(nLevel);

    
    int count = 0;
    for( auto &m : mobList)
    {
        m.setId(count);
        count++;
    }


    lastTime = 0.0f;
    curTime = 0.0f;

    gamestate = GameState::START;

    minTime = std::chrono::high_resolution_clock::time_point::min();
    Wpress = minTime;
    Apress = Wpress;
    Spress = Wpress;
    Dpress = Wpress;

    return true;
}

bool Gauntlet::OnUserUpdate(float fElapsedTime){


    // std::cout << GetMouseX() << " , " << GetMouseY() << std::endl;


    switch (gamestate)
    {

    case 0:
        // START
        // draw start screen
        // check if any button pressed to start game

        DrawDecal( {0.0f,0.0f} , utilDecal);
        
        for(int i=1; i<85; i++)
        {
            if( GetKey( (olc::Key) i ).bPressed )
            {
                gamestate = GameState::GAME;

                // no longer need utilDecal
                delete utilDecal;
                utilDecal = nullptr;
                
                cur_time = clock::now();
                prev_time = clock::now();

                break;
            }
        }



        break;

    case 1:
        // Game
        // do basic game loop
        // check if player has hit escape to pause game


        // accurate one second clock
        cur_time = clock::now();
        time_diff = std::chrono::duration_cast<ms>(cur_time - prev_time).count();
        if( time_diff >= 1000.0000000)
        {
            prev_time = cur_time;
            
            oneSecond = true;
            p1->setHp(p1->getHp() - 1.0f );

            // reset projShot
            projShot = false;
        }











        if(GetKey(olc::Z).bPressed)
            drawOutline = !drawOutline;

        /*
            GAME LOOP PROCESSES

            1.  GET INPUT FROM USER

            2.  COLLISION DETECTION ORDER
                a.  player vs tiles
                b.  proj vs tiles
                c.  mob vs tiles
                d.  player vs mob
                e.  mob vs projectiles
                f.  player vs projectiles


            3.  UPDATE ORDER
                a.  update player
                b.  update tiles
                c.  update mobs
                d.  update projectiles

            4.  DRAW ORDER
                a.  drawlevel
                b.  drawentities
        */


        // this will update the player based on the input keys W,A,S,D     

        updatePlayer(fElapsedTime);
        updateProjectiles(fElapsedTime);
        // updateMobs(fElapsedTime);

        // updateTiles(fElapsedTime);




        DrawLevel();
        DrawEntities();


        if(GetKey(olc::ESCAPE).bPressed){
            gamestate = GameState::PAUSED;
        }

        // reset the 1 sec timer
        oneSecond = false;

        break;

    case 2:
        // PAUSED
        // overlay pause screen give exit option and a resume option
        
        if(PauseChoice == -1)
            PauseChoice = 0;

        if(utilDecal == nullptr)
            utilDecal = new olc::Decal(Resources::get().getSprite("pauseScreen"));

        // 0 means resume 1 means exit
        if(PauseChoice == 0)
        {
            if(GetKey(olc::S).bPressed)
                PauseChoice = 1;
            else if(GetKey(olc::SPACE).bPressed){
                
                gamestate = GameState::GAME;
                delete utilDecal;
                utilDecal = nullptr;
                break;
            }
        }else if(PauseChoice == 1){

            if(GetKey(olc::W).bPressed)
                PauseChoice = 0;
            else if(GetKey(olc::SPACE).bPressed)
                bGameLoop = false;
        }

        DrawPartialDecal( { ScreenWidth()/2.0f - 40.0f, ScreenHeight()/2.0f - 26.0f }, utilDecal, { PauseChoice * 80.0f, 0.0f }, { 79.0f, 52.0f });
        break;

    case 3:
        // OVER
        // move the player to the exit and spin them few times then set GameLoop to false;
                
        DrawLevel();
        DrawEntities();

        if(ExitGameCounter == 0)
            bGameLoop = false;

        break;

    default:
        break;
    }

    // std::cout << mobList.size() << std::endl;

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
    delete outlineDecal;

    for(int i=0; i<vertTiles*horTiles; i++)
        delete gameObjects[i];
    delete[] gameObjects;

    return true;
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
        
        lineStream.str(line.erase(0,1));

        lineStream >> line;
        lineStream >> value;

        if(line == "height")
            vertTiles = value;        

        lineStream.clear();
        
        // width
        getline(fin, line);
        
        lineStream.str(line.erase(0,1));

        lineStream >> line;
        lineStream >> value;

        if(line == "width")
            horTiles = value;        

        lineStream.clear();
        

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

            if(type == 32 || type == 33)
                exitPos = { (float)i*16 - fTileOffsetX, (float)j*16 - fTileOffsetY};

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

    olc::vf2d screenPos = { (float)(ScreenWidth()/2.0f - offx) , (float)(ScreenHeight()/2.0f - offy) };
    
    if(gamestate == GameState::OVER)
    {   

        float angle = (ExitGameCounter%10) * (3.14/10.0f);
        DrawPartialRotatedDecal( exitPos + p1->getSize()/2.0f, decalList[1], angle, p1->getSize()/2.0f, {(float) p1->getDir()*16, 0.0f}, {16,16});
        ExitGameCounter--;

    }else{
        
        DrawPartialDecal( screenPos, decalList[1], { (float) p1->getDir()*16, (float) 16*frameBool}, {16, 16} );
        
        if(drawOutline)
            DrawDecal(screenPos, outlineDecal);
    }
        

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
                        DrawPartialDecal( { (float) mdx , (float) mdy } , decalList[ 2 ] , { (float) m.getDir()*16, (float)16*frameBool}, {16, 16} );     
                    else if(m.getType() == 36)
                        DrawPartialDecal( { (float) mdx , (float) mdy } , decalList[ 3 ] , { (float) m.getDir()*16, (float)16*frameBool}, {16, 16} );

                    if(drawOutline)
                        DrawDecal({ (float) mdx , (float) mdy }, outlineDecal);

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
                    
                    if(drawOutline)
                        DrawDecal({ (float) pdx , (float) pdy }, outlineDecal);
                }

        }
    }
}





void Gauntlet::updatePlayer(float fElapsedTime)
{

    olc::vf2d pPos = p1->getPosition(), vel = {0.0f, 0.0f};

    direction dir = p1->getDir();
    
    // get user input



    uint64_t dtNE= 0, dtNW = 0, dtSE = 0, dtSW = 0;

    uint64_t dt = 100;
    
    if(GetKey(olc::A).bHeld)
    {
        Apress = clock::now();
        vel += {-1.0f , 0.0f};
    }

    if(GetKey(olc::S).bHeld)
    {
        Spress = clock::now();
        vel += {0.0f , 1.0f};
    }

    if(GetKey(olc::D).bHeld)
    {
        Dpress = clock::now();
        vel += {1.0f , 0.0f};
    }

    if(GetKey(olc::W).bHeld)
    {
        Wpress = clock::now();
        vel += {0.0f , -1.0f};
    }

    // if(Wpress != std::chrono::time_point::min() && Apress != std::chrono::time_point::min())
    dtNW = std::chrono::duration_cast<ms>( std::chrono::abs(Wpress - Apress) ).count();
    dtSW = std::chrono::duration_cast<ms>( std::chrono::abs(Spress - Apress) ).count();
    dtNE = std::chrono::duration_cast<ms>( std::chrono::abs(Wpress - Dpress) ).count();
    dtSE = std::chrono::duration_cast<ms>( std::chrono::abs(Spress - Dpress) ).count();


    // if( abs(dtNE) <= dt && Wpress != Dpress)
    //     dir = direction::NE;
    // else if( abs(dtNW) <= dt && Wpress != Apress )
    //     dir = direction::NW;
    // else if( abs(dtSE) <= dt && Spress != Dpress )
    //     dir = direction::SE;
    // else if( abs(dtSW) <= dt && Spress != Apress )
    //     dir = direction::SW;










    if( vel.y < 0.0f )
       
        if( vel.x > 0.0f )
            dir = direction::NE;
        else if( vel.x < 0.0f )
             dir = direction::NW;
        else
            dir = direction::N;

    else if( vel.y > 0.0f )

        if( vel.x > 0.0f )
            dir = direction::SE;
        else if( vel.x < 0.0f )
             dir = direction::SW;
        else
            dir = direction::S;
    
    else if( vel.y == 0.0f )

        if(vel.x > 0.0f)
            dir = direction::E;
        else if(vel.x < 0.0f)
            dir = direction::W;







    p1->setDir(dir);
    // vel = vel * p1->getDirectionVector();

    // ==============================unit collision===========================

    vel *= p1->getSpeed();
    p1->setVelocity(vel);


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
        ResColl_DynamicVsTile(p1, fElapsedTime, gameObjects[j.first], j.first);


     // player vs mobs
    for(auto &m : mobList)
    {
        olc::vf2d mpos = m.getPosition();
        if( mpos.mag() - p1->getPosition().mag() < 16*8 )
        {
            ResColl_EntityVsDynamic(p1, fElapsedTime, &m);
        }
    }


    // create projectile on this command by player
    // if( oneSecond && (GetKey(olc::SPACE).bPressed || GetKey(olc::SPACE).bHeld || GetKey(olc::SPACE).bReleased) ){
    if( GetKey(olc::SPACE).bPressed && !projShot ){

        Proj p(37);

        projShot = true;
        p.dead = false;
        p.setDir(p1->getDir());

        vel = p1->getDirectionVector();

        p.setPosition(pPos + p1->getSize()/2.0f - p.getSize()/2.0f );
        p.setVelocity( vel * p.getSpeed());

        bool Overlap = false;
        for(int i=0; i<vertTiles*horTiles; i++)
        {
            if(gameObjects[i]->getType() != 19)
                Overlap = TileVsTile(&p, gameObjects[i]);

            if(Overlap)
                break;
        }

        if(Overlap == false)
            projList.push_back(p);

    }

    // update player pos
    p1->update(fElapsedTime);

}

void Gauntlet::updateMobs(float fElapsedTime){

    // std::cout << mobList.size() << std::endl;

    for(size_t i = 0; i < mobList.size(); i++)
    {

        if(mobList.at(i).dead)
        {
            mobList.erase(mobList.begin()+i);
        }

    }


    for( auto &m : mobList)
    {
        // dont waste time on dead mobs
        if(m.getHp() <= 0)
            m.dead = true;

        if(!m.dead)
        {
            
            // get direction to move from algorithm
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


            


            // unit collision

            m.setVelocity(vel);

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
                ResColl_DynamicVsTile(&m, fElapsedTime, gameObjects[j.first], j.first);



            // mobs vs other mobs

            for(int j=0; j< mobList.size(); j++)
            {
                
                // if(  m.getId() != mobList[j].getId())
                // {
                ResColl_EntityVsDynamic(&m, fElapsedTime, &mobList[j]);
                // }
            }

            // finally update the mob after handleing potential collisions
            m.update(fElapsedTime);

        }
    }
}

void Gauntlet::updateProjectiles(float fElapsedTime)
{   

    for(size_t i = 0; i < projList.size(); i++)
    {

        if(projList.at(i).dead)
        {
            projList.erase(projList.begin()+i);
        }

    }


    // ==============================unit collision===========================


    olc::vf2d pos, vel;

    for( auto &p : projList )
    {            
        if(!p.dead)
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
                ResColl_DynamicVsTile(&p, fElapsedTime, gameObjects[j.first], j.first);


            
            
            
            // projectiles vs mobs

            for(auto &m : mobList)
            {

                olc::vf2d mpos = m.getPosition();

                if( mpos.mag() - pos.mag() < 16*3 )
                {
                    ResColl_EntityVsDynamic(&m, fElapsedTime, &p);
                }

            }


            p.update(fElapsedTime);
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

            if(curTime - spawnRate >= 0)
            {

                spawnRate *= 2;
                
                // get spawner pos
                olc::vf2d mpos = gameObjects[i]->getPosition();
                int x = mpos.x, y = mpos.y;

                // check if a mob is to any of the sides of the spawner if not make that place the posiition to spawn mob

                int nidx, eidx, widx, sidx;

                nidx = i + -1*horTiles + 0;
                eidx = i + 0*horTiles + 1;
                sidx = i + 1*horTiles + 0;
                widx = i + 0*horTiles + -1;
                
                auto tileIsFloor = [&](int index)
                {                    
                    bool noMobs = true;

                    if(gameObjects[index]->getType() == 19)
                    {                     
                        for(auto &m : mobList)
                        {
                            if(  TileVsTile(&m, gameObjects[index])  )
                            {
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






bool Gauntlet::TileVsTile(const Tile* pt1, const Tile* pt2)
{
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

bool Gauntlet::RayVsDynamic(const olc::vf2d& ray_origin, const olc::vf2d& ray_dir, const Dynamic* target, const float fTimeStep, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& t_hit_near)
{

    // ray_origin is the expanded posistion
    // ray_dir is the entity velocity * fTimeStep
    // target is the Dynamic object 

    contact_normal = {0,0};
    contact_point = {0,0};

    olc::vf2d invdir = 1.0f / ray_dir;
    olc::vf2d t_near, t_far, pos, size, nextpos;

    // olc::vf2d invdir2 = 1.0f / ray_dir;
    // olc::vf2d 

    pos = target->getPosition();
    nextpos = pos + fTimeStep * target->getVelocity();
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

bool Gauntlet::EntityVsDynamic(const Entity* pEnt, const float fTimeStep, const Dynamic& pDyn, olc::vf2d& contact_point, olc::vf2d& contact_normal, float& contact_time)
{

    olc::vf2d entVel = pEnt->getVelocity();
    olc::vf2d dynVel = pDyn.getVelocity();

    // niether are moving so no collosion can happen
    // if(dynVel.mag() == 0.0f && entVel.mag() == 0.0f)
    //     return false;

    Dynamic expanded_target;
    expanded_target.setPosition( pDyn.getPosition() - pEnt->getSize() / 2.0f );
    expanded_target.setSize( pDyn.getSize() + pEnt->getSize() );
    expanded_target.setVelocity(pDyn.getVelocity());

    if(RayVsDynamic(pEnt->getPosition() + pEnt->getSize()/2.0f, entVel * fTimeStep, &expanded_target, fTimeStep, contact_point, contact_normal, contact_time))
        return (contact_time >= 0.0f && contact_time < 1.0f);
    else
        return false;

}

bool Gauntlet::ResColl_DynamicVsTile(Dynamic* pDyn, const float fTimeStep, Tile* pTile, int index)
{
    olc::vf2d contact_point, contact_normal;
    float contact_time = 0.0f;

    if(DynamicVsTile(pDyn, fTimeStep, *pTile, contact_point, contact_normal, contact_time))
    {
        int type = pDyn->getType(), tileType = pTile->getType();

        if(type == 34)
        {

            if(tileType == 20){
                
                // key
                p1->keyValue++;
                gameObjects[index]->setType(19);

            }else if((tileType == 21 || tileType == 22) && p1->keyValue > 0){


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



            }else if(tileType >= 32 && tileType <= 33){

                // exit
                gamestate = GameState::OVER;

            }else{

                // if not floor, needs to have a collision
                if(tileType != 19)
                {

                    // solidTile
                    olc::vf2d dynVel = pDyn->getVelocity();
                    dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
                    pDyn->setVelocity(dynVel.x, dynVel.y);

                }


            }

        }else if( type == 35 || type == 36 ){
        // mob

            if(tileType != 19)
            {

                // solidTile
                olc::vf2d dynVel = pDyn->getVelocity();
                dynVel += contact_normal * olc::vf2d(std::abs(dynVel.x) , std::abs(dynVel.y)) * (1 - contact_time);
                pDyn->setVelocity(dynVel.x, dynVel.y);

            }

        }else if( type >= 37 && type <= 40 ){
        // projectile 


            if( (tileType >= 0 && tileType <= 18) || (tileType >= 21 && tileType <= 22) )
            {
                // hits a wall
                pDyn->dead = true;


            }else if( tileType >= 26 && tileType <= 31 ){

                pDyn->dead = true;

                if(tileType == 26 || tileType == 29)
                    gameObjects[index]->setType(19);
                else
                    gameObjects[index]->setType(tileType-1);

            }

        } 

        return true;
    }


    return false;
}

bool Gauntlet::ResColl_EntityVsDynamic(Entity* pEnt, const float fTimeStep, Dynamic* pDyn)
{
    olc::vf2d contact_point, contact_normal;
    float contact_time = 0.0f;

    if(EntityVsDynamic(pEnt, fTimeStep, *pDyn, contact_point, contact_normal, contact_time))
    {

        int type = pEnt->getType(), dynType = pDyn->getType();

        if( type == 34){


            if((dynType == 35 || dynType == 36) && !pDyn->dead ){

                auto getMob = [&](int id){
                    
                    int count = 0;
                    for(auto &m : mobList){

                        if(EntityVsDynamic(pEnt, fTimeStep, m, contact_point, contact_normal, contact_time))
                            return count;
                        
                        count++;
                    }

                    return -1;

                };

                int idx = getMob(pDyn->getId());

                if(idx != -1)
                {
                    mobList[idx].dead = true;
                }
                
                pEnt->setHp(pEnt->getHp() - 15);

            }else if(dynType == 40){

                pDyn->dead = true;
                pEnt->setHp( pEnt->getHp() - 50 );

            }


        }else if( (type == 35 || type == 36) ){
            
            // mob vs mob
            if( (dynType == 35 || dynType == 36) && !pEnt->dead && !pDyn->dead)
            {

                // solidTile
                olc::vf2d entVel = pEnt->getVelocity();
                entVel += contact_normal * olc::vf2d(std::abs(entVel.x) , std::abs(entVel.y)) * (1 - contact_time);
                pEnt->setVelocity(entVel);


            }else if(dynType >= 37 && dynType <= 39){

                // proj is dead
                pDyn->dead = true;
                pEnt->setHp(pEnt->getHp() - 100);
                
                if(pEnt->getHp() <= 0)
                    pEnt->dead = true;

            }



        }

        return true;
    }

    return false;
}










