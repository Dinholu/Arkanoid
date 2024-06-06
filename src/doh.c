#include "doh.h"

Doh doh;
SDL_Rect srcEnemyBall = {96, 64, 24, 24};

void initializeDoh()
{
    SDL_Rect srcDoh = {0, 144, 64, 96};
    doh.height = 256;
    doh.width = 192;
    doh.x = win_surf->w / 2 - doh.width / 2;
    doh.y = srcTopWall.h + Y_WALLS + doh.height / 4;
    doh.type = 'X';
    doh.health = 5;
    doh.scoreValue = 1000;
    doh.animationPhase = 0;
    doh.phaseStartTime = SDL_GetPerformanceCounter();
    doh.moveDown = false;
    doh.hasMovedDown = false;
    doh.moveStartTime = SDL_GetPerformanceCounter();
    doh.disappearing = false;
    doh.animationFrame = 0;
    doh.lastFrameTime = SDL_GetPerformanceCounter();
    doh.moveDownTime = 0;
}
