#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef ARKANOID_BALL_H
#define ARKANOID_BALL_H

#include "brick.h"
#include "vault.h"
#include "harmful.h"
#include "bonus.h"
#include "render.h"

#define BALL_SPEED_INCREMENT 1.0 // Speed increment when hitting a brick
#define MAX_BALLS 3

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
} ball;

struct Ball balls[MAX_BALLS];
SDL_Rect srcBall = {63, 65, 16, 16};
Uint64 attachTime = 0;
int releaseCount = 0;
int activeBallCount = 1;
double ballSpeedIncrement = BALL_SPEED_INCREMENT;
double max_speed = 7.0;

void wallCollision(struct Ball *ball);
void vaultCollision(struct Ball *ball);
void handleBallProperty(struct Ball *ball, SDL_Rect brickRect);
void handleCollisions();
void brickCollision(struct Ball *ball);
void defeatCollision(struct Ball *ball);
void initializeBalls();
void attachBallToVault(struct Ball *ball, int x_vault);

#endif // ARKANOID_BALL_H
