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

    decalList[0] = new olc::Decal(Resources::get().getSprite("lvlSheet"));
    decalList[1] = new olc::Decal(Resources::get().getSprite("player"));
    decalList[2] = new olc::Decal(Resources::get().getSprite("ghost"));
    decalList[3] = new olc::Decal(Resources::get().getSprite("demon"));
    decalList[4] = new olc::Decal(Resources::get().getSprite("p1Proj"));

    infoDecal1 = new olc::Decal(Resources::get().getSprite("info1"));
    infoDecal2 = new olc::Decal(Resources::get().getSprite("info2"));
    utilDecal = new olc::Decal(Resources::get().getSprite("startScreen"));
    //outlineDecal = new olc::Decal(Resources::get().getSprite("outline"));


    p1 = new Player();
    p1->setSpeed(100);
    p1->setHp(500);
    
    
    nLevel = 1;
    loadLevel(nLevel);

    
    int count = 0;
    for( auto &m : mobList)
    {
        m.setId(count);
        count++;
    }


    gamestate = GameState::START;

    minTime = std::chrono::high_resolution_clock::time_point::min();
    Wpress = minTime;
    Apress = Wpress;
    Spress = Wpress;
    Dpress = Wpress;
    Wrelease = Wpress;
    Arelease = Wpress;
    Srelease = Wpress;
    Drelease = Wpress;


    return true;
}

bool Gauntlet::OnUserUpdate(float fElapsedTime){

    
    // if user does not have view on the application window, the game will be paused
    if (gamestate == GameState::GAME && !IsFocused())
        gamestate = GameState::PAUSED;


    if (gamestate == GameState::GAME)
    {

        // Physics  - timing, Fixed updates, manage collisions
        cur_time = clock::now();
        time_diff = std::chrono::duration_cast<ms>(cur_time - prev_time).count();
        if (time_diff >= 1000.0000)
        {
            prev_time = cur_time;
            p1->setHp(p1->getHp() - 1.0f);

            // reset projShot
            projShot = false;
        }

        // handles all collisions
        CheckCollisions(fElapsedTime);

        // Get the offx and offy for any screen to world or visversa calls
        GetScreenWorldBounds();


        ////////////////////////////////////////////////////////////////////////////////////


        // Render Game Data
        DrawLevel();
        DrawInfo();
        DrawEntities();


        ///////////////////////////////////////////////////////////////////////////////////


        // Game Logic
        updateTiles(fElapsedTime);
        p1->update(fElapsedTime);
        updateProjectiles(fElapsedTime);
        updateMobs(fElapsedTime);


        ///////////////////////////////////////////////////////////////////////////////////


        // Input Events
        GatherInputs(); // movement, create projectiles, use ability

    }
    else if (gamestate == GameState::PAUSED)
    {

        if (utilDecal == nullptr)
            utilDecal = new olc::Decal(Resources::get().getSprite("pauseScreen"));

        // 0 means resume 1 means exit
        if (PauseChoice == 0)
        {
            if (GetKey(olc::S).bPressed)
                PauseChoice = 1;
            else if (GetKey(olc::SPACE).bPressed) {

                gamestate = GameState::GAME;
                
            }
        }
        else if (PauseChoice == 1) {

            if (GetKey(olc::W).bPressed)
                PauseChoice = 0;
            else if (GetKey(olc::SPACE).bPressed)
                bGameLoop = false;
        }

        DrawPartialDecal({ ScreenWidth() / 2.0f - 40.0f, ScreenHeight() / 2.0f - 26.0f }, utilDecal, { PauseChoice * 80.0f, 0.0f }, { 79.0f, 52.0f });

        if (gamestate == GameState::GAME)
        {
            delete utilDecal;
            utilDecal = nullptr;
        }

    }
    else if (gamestate == GameState::START)
    {

        // draw start screen
        // check if any button pressed to start game
        DrawDecal({ 0.0f,0.0f }, utilDecal);

        for (int i = 1; i < 85; i++)
        {
            if (GetKey((olc::Key) i).bPressed)
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

    }
    else if (gamestate == GameState::OVER)
    {

        DrawLevel();
        DrawEntities();

        if (ExitGameCounter == 0)
            bGameLoop = false;
    }

    return bGameLoop;
}

bool Gauntlet::OnUserDestroy(){

    // makeing sure to clean up all created memory for no memory leaks!

    delete p1;

    for(int i=0; i<decalNum; i++)
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
        

        gameObjects = new Tile* [ horTiles * vertTiles];
        
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


    // setting screen world bounds
    screen_bound_BOT = ( nVisTilesY + fOffsetY ) * 16;
    screen_bound_TOP = fOffsetY * 16;
    screen_bound_RIGHT = ( nVisTilesX + fOffsetX ) * 16;
    screen_bound_LEFT = fOffsetX * 16;


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

}

void Gauntlet::DrawInfo() 
{

    // display health/timer, keys, and potions
    DrawDecal({ 16 * 2,16 * 2 }, infoDecal1);
    DrawStringDecal({ 16 * 2 + 8,16 * 2 }, std::to_string(p1->getHp()), olc::WHITE, { 1.0f, 1.0f });
    DrawStringDecal({ 16 * 2 + 8,16 * 2 + 8 }, std::to_string(p1->keyValue), olc::WHITE, { 1.0f, 1.0f });
    DrawStringDecal({ 16 * 2 + 24,16 * 2 + 8 }, std::to_string(p1->potions), olc::WHITE, { 1.0f, 1.0f });

    // display world level
    char buffer[12];                            // used 12 here because "%02d" has a max value of 12 chars
    sprintf(buffer, "%02d", nLevel);

    DrawDecal({ ScreenWidth() / 2.0f, 24 }, infoDecal2);
    DrawStringDecal({ ScreenWidth() / 2.0f, 24 }, std::string(buffer), olc::WHITE, { 1.0f, 1.0f });

    // debug info
    //olc::vf2d mouse = { (float) GetMouseX(), (float) GetMouseY()};
    //std::cout << mouse.x << "  ,  " << mouse.y << std::endl;

}

void Gauntlet::DrawEntities(){


    // flicker for sprite animation timer
    
    if (std::chrono::duration_cast<ms>(clock::now() - mob_flicker).count() >= 500) 
    {
        mob_flicker = clock::now();
        frameBool = !frameBool;
    }


    static bool pbool = false;
    if (std::chrono::duration_cast<ms>(clock::now() - player_flicker).count() >= 50 && p1->getVelocity().mag2() > 0.0f)
    {
        player_flicker = clock::now();
        pbool = !pbool;
    }


    // Draw Player

    olc::vf2d PlayerScreenPos, pos = p1->getPosition();

    WorldToScreen(pos.x, pos.y, PlayerScreenPos.x, PlayerScreenPos.y);

    if(gamestate == GameState::OVER)
    {   

        float angle = (ExitGameCounter%10) * (3.14/10.0f);
        DrawPartialRotatedDecal( exitPos + p1->getSize()/2.0f, decalList[1], angle, p1->getSize()/2.0f, {(float) p1->getDir()*16, 0.0f}, {16,16});
        ExitGameCounter--;

    }else{
        
        DrawPartialDecal( PlayerScreenPos, decalList[1], { (float) p1->getDir()*16, (float) 16*pbool}, {16, 16} );
        
        if(drawOutline)
            DrawDecal(PlayerScreenPos, outlineDecal);
    }
        




    // draw mobs and projectiles
    for(auto &m : mobList){

        if(m.dead == false)
        {
            olc::vf2d mobPos = m.getPosition();
            olc::vf2d ScreenPos;

            WorldToScreen(mobPos.x, mobPos.y, ScreenPos.x, ScreenPos.y);

            if(ScreenPos.x >= 0 && ScreenPos.x <= ScreenWidth())
                if(ScreenPos.y >= 0 && ScreenPos.y <= ScreenHeight()){

                    DrawPartialDecal( ScreenPos, decalList[ m.getType() - 33 ] , { (float) m.getDir()*16, (float)16*frameBool}, {16, 16} );     

                    if(drawOutline)
                        DrawDecal(ScreenPos, outlineDecal);

                }
        }
    }



    for(auto &p : projList){

        if(p.dead == false)
        {
            olc::vf2d pPos = p.getPosition();
            olc::vf2d ScreenPos;

            WorldToScreen(pPos.x, pPos.y, ScreenPos.x, ScreenPos.y);

            if (ScreenPos.x >= 0 && ScreenPos.x <= ScreenWidth())
                if (ScreenPos.y >= 0 && ScreenPos.y <= ScreenHeight()) {

                    DrawPartialDecal(ScreenPos, decalList[4], { (float)p.getDir() * 16,0 }, { 16, 16 });

                    if (drawOutline)
                        DrawDecal(ScreenPos, outlineDecal);

                }

        }
    }
}




void Gauntlet::CheckCollisions(float fElapsedTime)
{
    
    // collision check order ( doesnt really matter since it all happens before render and update , just how effecient is the question)
    /// player vs tiles
    /// mobs vs tiles           -- wont need this because mobs will have different algorithm for movement, game logic will handle this
    /// projectiles vs tiles
    /// 
    /// mobs vs player
    /// mobs vs projectiles


    /// here we make a general lambda function which checks the tiles around a dynamic object
    /// within a 2 block radius
    auto CheckDynamicVsTiles = [&](Dynamic* pDyn) 
    {
        
        // check for collisiosns then sort collisions by distance
        olc::vf2d cp, cn, pos = pDyn->getPosition();
        float t = 0, min_t = INFINITY;
        std::vector<std::pair<int, float>> z;

        int left_bound, right_bound;
        int top_bound, bot_bound;

        left_bound = int(pos.x / 16) - 2;
        right_bound = int(pos.x / 16) + 2;
        top_bound = int(pos.y / 16) - 2;
        bot_bound = int(pos.y / 16) + 2;

        if (left_bound < 0) left_bound = 0;
        if (right_bound > horTiles) right_bound = horTiles;
        if (top_bound < 0) top_bound = 0;
        if (bot_bound > vertTiles) bot_bound = vertTiles;


        for (int i = left_bound; i < right_bound; i++) {
            for (int j = top_bound; j < bot_bound; j++)
            {

                if (DynamicVsTile(pDyn, fElapsedTime, *gameObjects[j * horTiles + i], cp, cn, t))
                {
                    z.push_back({ j * horTiles + i,t });
                }
            }
        }

        // sort the list z by distance 
        std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
            {
                return a.second < b.second;
            });

        // resolve collisions in proper order 
        for (auto j : z)
            ResColl_DynamicVsTile(pDyn, fElapsedTime, gameObjects[j.first], j.first);

    };

    // using the lambda CheckDynVsTiles we can just call it to handle our collisions for tiles

    // player vs tiles
    CheckDynamicVsTiles(p1);
    // projs vs tiles
    for (auto& p : projList)
        CheckDynamicVsTiles(&p);
    

    // mob collisions
    for (auto& m : mobList)
    {
        // mobs vs player
        if (TileVsTile(&m, p1))
        {
            Resolve_MobVsDynamic(&m, fElapsedTime, p1);
        }

        // mobs vs projectiles
        for (auto& p : projList) 
        {
            if (TileVsTile(&m, &p))
            {
                Resolve_MobVsDynamic(&m, fElapsedTime, &p);
            }
        }
    }


}

void Gauntlet::GatherInputs() 
{
    // get user input
    olc::vf2d vel = {0.0f , 0.0f};
    direction dir, old_dir = p1->getDir();

    uint64_t dtNE = 0, dtNW = 0, dtSE = 0, dtSW = 0;
    uint64_t dt = 35;

    // sets the *press vars to current wall time
    if (GetKey(olc::A).bPressed)
    {
        Apress = clock::now();
    }
    if (GetKey(olc::S).bPressed)
    {
        Spress = clock::now();
    }
    if (GetKey(olc::D).bPressed)
    {
        Dpress = clock::now();
    }
    if (GetKey(olc::W).bPressed)
    {
        Wpress = clock::now();
    }

    // if key is being held we can just add the velocities and determine direction (main source of input)
    if (GetKey(olc::A).bHeld)
    {
        vel += {-1.0f, 0.0f};
        dir = direction::W;
    }
    if (GetKey(olc::S).bHeld)
    {
        vel += {0.0f, 1.0f};
        dir = direction::S;
    }
    if (GetKey(olc::D).bHeld)
    {
        vel += {1.0f, 0.0f};
        dir = direction::E;
    }
    if (GetKey(olc::W).bHeld)
    {
        vel += {0.0f, -1.0f};
        dir = direction::N;
    }

    // sets the *release vars to current wall time
    if (GetKey(olc::A).bReleased)
    {
        Arelease = clock::now();
    }
    if (GetKey(olc::S).bReleased)
    {
        Srelease = clock::now();
    }
    if (GetKey(olc::D).bReleased)
    {
        Drelease = clock::now();
    }
    if (GetKey(olc::W).bReleased)
    {
        Wrelease = clock::now();
    }



    // calculate released times to see if direction needs to be modified
    // use this to basically allow for fuzzy timing with control inputs
    dtNW = std::chrono::duration_cast<ms>(std::chrono::abs(Wrelease - Arelease)).count();
    dtSW = std::chrono::duration_cast<ms>(std::chrono::abs(Srelease - Arelease)).count();
    dtNE = std::chrono::duration_cast<ms>(std::chrono::abs(Wrelease - Drelease)).count();
    dtSE = std::chrono::duration_cast<ms>(std::chrono::abs(Srelease - Drelease)).count();


    // here we can get the direction if the HELD conditions were met
    if (vel.y < 0.0f) {

        if (vel.x > 0.0f)
            dir = direction::NE;
        else if (vel.x < 0.0f)
            dir = direction::NW;
        else
            dir = direction::N;

    }
    else if (vel.y > 0.0f) {

        if (vel.x > 0.0f)
            dir = direction::SE;
        else if (vel.x < 0.0f)
            dir = direction::SW;
        else
            dir = direction::S;

    }
    else if (vel.y == 0.0f) {

        if (vel.x > 0.0f)
            dir = direction::E;
        else if (vel.x < 0.0f)
            dir = direction::W;
        else /*if( vel.x == 0.0f )*/ 
            dir = direction::NONE;

    }

    
    // todo: make sure it is most recent
    //uint64_t arr[4] = { dtNW, dtSW, dtNE, dtSE };
    int index = -1;

    // gets the most recent pair of keys released
    if (Wrelease > Srelease)
    {
        // w after s
        if (Arelease < Drelease)
        {
            // D after A
            //NE
            index = 0;
        }
        else {
            index = 1;
        }
    }
    else
    {
        // w before s
        if (Arelease < Drelease)
        {
            // D before A 
            index = 2;
        }
        else {
            index = 3;
        }
    }


    if (index == 0 && dtNE <= dt && Wrelease != minTime && Drelease != minTime)
    {
        dir = direction::NE;

    }
    if (index == 1 && dtNW <= dt && Wrelease != minTime && Arelease != minTime) {

        dir = direction::NW;

    }
    if (index == 2 && dtSE <= dt && Srelease != minTime && Drelease != minTime) {

        dir = direction::SE;

    }
    if (index == 3 && dtSW <= dt && Srelease != minTime && Arelease != minTime) {

        dir = direction::SW;

    }


    // this means the velocity was not 0
    if (dir != direction::NONE)
    {
        p1->setDir(dir);
        //vel = p1->getDirectionVector();
    }
    else
    {
        p1->setDir(old_dir);
    }

    vel *= p1->getSpeed();
    p1->setVelocity(vel);


    // create projectile on this command by player
    // if( oneSecond && (GetKey(olc::SPACE).bPressed || GetKey(olc::SPACE).bHeld || GetKey(olc::SPACE).bReleased) ){
    if ( GetKey(olc::SPACE).bPressed ) {

        // && !projShot --> is onesec timer basically

        int projCount = 0;
        for (size_t i = 0; i < projList.size(); i++) 
        {
            int pType = projList.at(i).getType();

            if (pType >= 36 && pType <= 40 && !projList.at(i).dead) 
            {
                projCount++;
            }
        }

        if (projCount == 0 || !projShot)
        {
            Proj p(37);

            projShot = true;
            p.dead = false;
            p.setDir(p1->getDir());

            vel = p1->getDirectionVector();

            p.setPosition(p1->getPosition() + p1->getSize() / 2.0f - p.getSize() / 2.0f);
            p.setVelocity(vel* p.getSpeed());

            bool Overlap = false;
            for (int i = 0; i < vertTiles * horTiles; i++)
            {
                if (gameObjects[i]->getType() != 19)
                    Overlap = TileVsTile(&p, gameObjects[i]);

                if (Overlap)
                    break;
            }

            if (Overlap == false)
                projList.push_back(p);

        }

    }



    if (GetKey(olc::ESCAPE).bPressed) {
        gamestate = GameState::PAUSED;
    }

}



void Gauntlet::GetScreenWorldBounds()
{
        
    olc::vf2d playerPosition = p1->getPosition();
    int nVisTilesX = ScreenWidth() / 16;
    int nVisTilesY = ScreenHeight() / 16;

    offx = 0, offy = 0;
    float xScreenOffset = nVisTilesX / 2.0f;
    float yScreenOffset = nVisTilesY / 2.0f;

    if (playerPosition.x < xScreenOffset * 16)
        offx = xScreenOffset * 16 - playerPosition.x;
    else if (playerPosition.x > (horTiles - xScreenOffset) * 16)
        offx = (horTiles - xScreenOffset) * 16 - playerPosition.x;

    if (playerPosition.y < yScreenOffset * 16)
        offy = yScreenOffset * 16 - playerPosition.y;
    else if (playerPosition.y > (vertTiles - yScreenOffset) * 16)
        offy = (vertTiles - yScreenOffset) * 16 - playerPosition.y;


    offx = ScreenWidth() / 2.0f - offx - playerPosition.x;
    offy = ScreenHeight() / 2.0f - offy - playerPosition.y;

}

void Gauntlet::ScreenToWorld(float sx, float sy, float& wx, float& wy)
{
    wx = sx - offx;
    wy = sy - offy;

}

void Gauntlet::WorldToScreen(float wx, float wy, float& sx, float& sy) 
{   
    sx = wx + offx;
    sy = wy + offy;
}




void Gauntlet::updateMobs(float fElapsedTime){

    // a timer to move the mobs
    mob_current_time = clock::now();
    uint64_t mob_dt = std::chrono::duration_cast<ms>(std::chrono::abs(mob_current_time - mob_last_time)).count();

    // compresses the mob vector
    for(size_t i = 0; i < mobList.size(); i++)
    {

        if (mobList.at(i).getHp() <= 0)
            mobList.at(i).dead = true;

        if(mobList.at(i).dead)
        {
            mobList.erase(mobList.begin()+i);
        }

    }


    // z.first() holds the index, z.second holds the distance to player
    std::vector<std::pair<int, float>> z;
    for (size_t i = 0; i < mobList.size(); i++)
    {
        float dx = mobList.at(i).getPosition().mag() -  p1->getPosition().mag();
        dx = std::abs(dx);

        z.push_back( {(int)i, dx} );
    }

    std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
        {
            return a.second < b.second;
        });




    olc::vf2d ppos = p1->getPosition();

    for( auto j : z )
    {
        // get direction to move from algorithm
        olc::vf2d pos = mobList.at(j.first).getPosition(), vel = { 0.0f, 0.0f }, moveDir;

        
        // see if mob is on the screen
        bool onScreen = false;
        olc::vf2d screenPos;
        WorldToScreen(pos.x, pos.y, screenPos.x, screenPos.y);
        
        if (screenPos.x >= 0 && screenPos.x <= ScreenWidth())
            if (screenPos.y >= 0 && screenPos.y <= ScreenHeight())
                onScreen = true;


 
        // determine move direction    
        moveDir = ppos - pos;
            
        if( onScreen )
        {
            mobList.at(j.first).setVelocity(moveDir);
            direction dir = mobList.at(j.first).getVelocityDirection();
            mobList.at(j.first).setDir(dir);
            mobList.at(j.first).setVelocity(vel);
            vel = mobList.at(j.first).getDirectionVector();

            if (std::abs(moveDir.x) < 7)
                vel.x = 0.0f;
            if (std::abs(moveDir.y) < 7)
                vel.y = 0.0f;

            mobList.at(j.first).setVelocity(vel);
            dir = mobList.at(j.first).getVelocityDirection();
            mobList.at(j.first).setDir(dir);



            vel *= 8.0f / fElapsedTime;

            olc::vf2d nextp = pos + vel * fElapsedTime;
            olc::vf2d nextp_xonly = pos + olc::vf2d(vel.x, 0.0f) * fElapsedTime;
            olc::vf2d nextp_yonly = pos + olc::vf2d(0.0f, vel.y) * fElapsedTime;

            auto PosCollision = [&](olc::vf2d pos)
            {

                int x0 = (int)pos.x, y0 = (int)pos.y;
                Mob* mTile = new Mob(35, pos.x, pos.y);

                int left_bound, right_bound;
                int top_bound, bot_bound;

                left_bound = int(pos.x / 16) - 2;
                right_bound = int(pos.x / 16) + 2;
                top_bound = int(pos.y / 16) - 2;
                bot_bound = int(pos.y / 16) + 2;

                if (left_bound < 0) left_bound = 0;
                if (right_bound > horTiles) right_bound = horTiles;
                if (top_bound < 0) top_bound = 0;
                if (bot_bound > vertTiles) bot_bound = vertTiles;


                for (int i = left_bound; i < right_bound; i++) {
                    for (int j = top_bound; j < bot_bound; j++)
                    {

                        if (TileVsTile(gameObjects[j * horTiles + i], mTile) && gameObjects[j * horTiles + i]->getType() != 19)
                        {
                            delete mTile;
                            return true;
                        }

                    }
                }

                for (auto j2 : z)
                {
                    if (j2.first != j.first)
                    {
                        if (TileVsTile(mTile, &mobList.at(j2.first)))
                        {
                            delete mTile;
                            return true;
                        }
                    }
                }
                
                delete mTile;
                return false;
            };


            bool can_move_x = !PosCollision(nextp_xonly);
            bool can_move_y = !PosCollision(nextp_yonly);

            // prio is always XY because it is the closest distance
            if (PosCollision(nextp))
            {

                if (can_move_x)
                {
                    if (can_move_y)
                    { 
                        // pick the direction which makes distance the shortest
                        if ((ppos - nextp_xonly).mag() < (ppos - nextp_yonly).mag())
                        {
                            vel = { vel.x , 0.0f };
                        }
                        else
                        {
                            vel = { 0.0f, vel.y };
                        }
                    }
                    else 
                    {
                        vel = { vel.x , 0.0f };
                    }

                }
                else if (can_move_y)
                {
                    vel = {0.0f, vel.y};
                }
                else
                {
                    vel = {0.0f, 0.0f};
                }
            }

            // handel the constant moving back and forth for mobs 
            float cur_dx = (ppos - pos).mag();
            float next_dx = (ppos - (pos + vel * fElapsedTime)).mag();
            
            if ( cur_dx <=  next_dx )
            {
                vel = {0.0f,0.0f};
            }

        }

        mobList.at(j.first).setVelocity(vel);

        // finally update the mob after handleing potential collisions
        // if the move time is good then move to the next position possible
        if (mob_dt >= 500)
        {
            mob_last_time = mob_current_time;
            // this will actually move the mob based on its set vel
            mobList.at(j.first).update(fElapsedTime);
        }

        
    }
}

void Gauntlet::updateProjectiles(float fElapsedTime)
{   

    // compress the projList vec
    for(size_t i = 0; i < projList.size(); i++)
    {
        if(projList.at(i).dead)
        {
            projList.erase(projList.begin()+i);
        }
    }

    for( auto &p : projList )
    {            
        p.update(fElapsedTime);
    }

}

void Gauntlet::updateTiles(float fElapsedTime){

    for(int i=0; i < vertTiles*horTiles; i++)
    {
        olc::vf2d tpos = gameObjects[i]->getPosition();
        int tileType = gameObjects[i]->getType();

        if(tileType >= 26 && tileType <= 31 && (tpos - p1->getPosition()).mag() <= 16 * 13 )
        {

            int mType = 0;
            if(tileType >= 26 && tileType <= 28)
                mType = 35;
            else if(tileType >= 29 && tileType <= 31)
                mType = 36;

            // make new mob in a postion 1 tile away from current tile position

            if(std::chrono::duration_cast<ms>(clock::now() - spawnrate).count() >= 1000)
            {
                
                spawnrate = clock::now();

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
                    bool isFloor = true;

                    if(gameObjects[index]->getType() == 19)
                    {                     
                        for(auto &m : mobList)
                        {
                            if(  TileVsTile(&m, gameObjects[index])  )
                            {
                                isFloor = false;
                                break;
                            }
                        }
                    }
                    else
                    {
                        isFloor = false;
                    }

                    return isFloor;
                };


                int indexes[4] = {nidx, eidx, sidx, widx};

                for (auto& i : indexes)
                {

                    if (tileIsFloor(i))
                    {
                        olc::vf2d pos = gameObjects[i]->getPosition();
                        Mob m(mType, pos.x, pos.y);
                        mobList.push_back(m);
                        break;
                    }

                }
                              
                
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

bool Gauntlet::Resolve_MobVsDynamic(Mob* m, const float fTimeStep, Dynamic* pDyn)
{
    int mType = m->getType();
    int pType = pDyn->getType();

    if (pType == 34 && m->dead == false)
    {
        // player collision
        p1->setHp(p1->getHp() - 15);
        m->dead = true;
    }
    else if (pType >= 37 && pType <= 40 && pDyn->dead == false && m->dead == false)
    {
        // player projectile
        if (mType == 35) 
        {
            pDyn->dead = true;
            m->dead = true;
        }
        else if (mType == 36)
        {
            pDyn->dead = true;
            m->setHp(m->getHp() - 50);
            
            if (m->getHp() <= 0)
                m->dead = true;
        }
    }

    return true;
}







