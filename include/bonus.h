// BONUS_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef ARKANOID_BONUS_H
#define ARKANOID_BONUS_H

#include "vault.h"
#include "score.h"
#include "game.h"

#define MAX_LASERS 10
#define MAX_BONUSES 10

struct Bonus
{
    double x;
    double y;
    double vy;
    bool isActive;
    int type;
    int animationFrame;
    double animationTime;
};
struct Bonus bonuses[MAX_BONUSES];

struct Laser
{
    double x;
    double y;
    double vy;
    bool isActive;
    bool hasTouchedBrick;
    bool isAnimating;
    int animationFrame;
    Uint64 lastFrameTime;
} lasers[MAX_LASERS];

SDL_Rect srcLeftLaser = {0, 80, 16, 20};
SDL_Rect srcRightLaser = {16, 80, 16, 20};

bool isVaultEnlarged = false;
bool isEnlarging = false;
bool isShrinking = false;

bool isLaserBeam = false;

int enlargeSteps = 5;
int currentStep = 0;
Uint64 enlargeStartTime = 0;
double enlargeDuration = 0.5;
double enlargedHoldDuration = 5.0;
double shrinkDuration = 0.5;

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