#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef ARKANOID_BONUS_H
#define ARKANOID_BONUS_H

#include "ball.h"
#include "brick.h"
#include "vault.h"
#include "harmful.h"
#include "render.h"

// slow down ball
#define S_BONUS (SDL_Rect) BRICK(256, 0) // pour animer ca doit faire (288, 0) puis (320, 0) puis (352, 0) et cela 8 x puis reprendre a (256, 0)
// catch and fire
#define C_BONUS (SDL_Rect) BRICK(256, 16)
// laser beam
#define L_BONUS (SDL_Rect) BRICK(256, 32)
// enlarge vault
#define E_BONUS (SDL_Rect) BRICK(256, 48)
// split ball
#define D_BONUS (SDL_Rect) BRICK(256, 64)
// wrap level
#define B_BONUS (SDL_Rect) BRICK(256, 80)
// add life
#define P_BONUS (SDL_Rect) BRICK(256, 96)
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
// bonus enlarge le vaisseau

int enlargeSteps = 5;
int currentStep = 0;
Uint64 enlargeStartTime = 0;
double enlargeDuration = 0.5;      // Durée de l'agrandissement en secondes
double enlargedHoldDuration = 5.0; // Durée pendant laquelle le vaisseau reste agrandi en secondes
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