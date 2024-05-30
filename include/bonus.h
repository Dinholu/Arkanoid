#ifndef ARKANOID_BONUS_H
#define ARKANOID_BONUS_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "vault.h"
#include "score.h"
#include "game.h"

#define MAX_LASERS 10
#define MAX_BONUSES 10

struct Bonus {
    double x;
    double y;
    double vy;
    bool isActive;
    int type;
    int animationFrame;
    double animationTime;
};
extern struct Bonus bonuses[MAX_BONUSES];

struct Laser {
    double x;
    double y;
    double vy;
    bool isActive;
    bool hasTouchedBrick;
    bool isAnimating;
    int animationFrame;
    Uint64 lastFrameTime;
};
extern struct Laser lasers[MAX_LASERS];

extern SDL_Rect srcLeftLaser;
extern SDL_Rect srcRightLaser;

extern bool isVaultEnlarged;
extern bool isEnlarging;
extern bool isShrinking;
extern bool isLaserBeam;
extern int enlargeSteps;
extern int currentStep;
extern Uint64 enlargeStartTime;
extern double enlargeDuration;
extern double enlargedHoldDuration;
extern double shrinkDuration;

void fireLaser();
void initializeBonuses();
void initializeLasers();
void splitBall();
void resetAllBonuses();
void clearBonuses();
void enlargeVault();
void updateVaultEnlargement();
void addLife();
void slowDownBall();
void CatchAndFire();
void wraplevel();
void handleBonusCollision();

#endif // ARKANOID_BONUS_H
