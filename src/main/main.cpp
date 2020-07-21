#include "olcPixelGameEngine.h" 
#include "gauntlet.h"

const int t_width   = 256;
const int t_height  = 240;

int main(){
    
    Gauntlet testExample;

    if(testExample.Construct(t_width, t_height, 4,4))
        testExample.Start();

    return 0;
}