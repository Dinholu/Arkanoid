#include "doh.h"

Doh doh;

void initializeDoh()
{
    doh.height = 256;
    doh.width = 192;
    doh.x = win_surf->w / 2 - doh.width / 2;
    doh.y = srcTopWall.h + Y_WALLS + doh.height / 2;
    doh.type = 'X';
    doh.health = 10;
    doh.scoreValue = 1000;
    doh.animationPhase = 0;
    doh.phaseStartTime = SDL_GetPerformanceCounter();
}
