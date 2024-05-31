// BALL_H
#ifndef BALL_H
#define BALL_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "vault.h"
#include "brick.h"
#include "render.h"

#define BALL_SPEED_INCREMENT 0.5 // Speed increment when hitting a brick
#define MAX_BALLS 50

struct Ball
{
    double x;
    double y;
    double vx;
    double vy;
    bool isActive;
    bool isAttached;
    Uint64 attachTime;
    int relative;
};

extern struct Ball balls[MAX_BALLS];
extern SDL_Rect srcBall;
extern Uint64 attachTime;
extern int releaseCount;
extern int activeBallCount;
extern double ballSpeedIncrement;
extern double max_speed;

void wallCollision(struct Ball *ball);
void vaultCollision(struct Ball *ball);
void handleBallProperty(struct Ball *ball, SDL_Rect brickRect);
void handleCollisions();
void brickCollision(struct Ball *ball);
void defeatCollision(struct Ball *ball);
void initializeBalls();
void attachBallToVault(struct Ball *ball, int x_vault);

#endif // BALL_H
