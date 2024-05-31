#ifndef ARKANOID_HARMFUL_H
#define ARKANOID_HARMFUL_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "ball.h"
#include "bonus.h"
#include "brick.h"
#include "game.h"
#include "render.h"
#include "score.h"
#include "vault.h"

#define HARMFUL_2 (SDL_Rect) { 0, 256, 32, 32 }
#define HARMFUL_3 (SDL_Rect) { 0, 288, 32, 32 }
#define HARMFUL_5 (SDL_Rect) { 0, 320, 32, 32 }
#define HARMFUL_EXPLOSITION (SDL_Rect) { 0, 384, 32, 32 }
#define MAX_HARMFULS 5

struct Harmful {
    double x;
    double y;
    double vx;
    double vy;
    bool isActive;
    int type;
    bool isFalling;
    double time;
    int height;
    float elongationTime;
    double initialY;
    double amplitude;
    bool isSinusoidal;
    int animationFrame;
    double animationTime;
    float randomElongationTime;
    int maxSteps;
    bool isDestroying;
    int destroyAnimationFrame;
    double destroyAnimationTime;
};
extern struct Harmful harmfuls[MAX_HARMFULS];

extern int currentHarmfulsType;

void initializeHarmfuls();
void generateHarmfuls();
SDL_Rect getHarmfulSrcRect(int type, int frame);
void addRandomHarmfuls();
void handleHarmfulCollisions();
void clearHarmfuls();

#endif // ARKANOID_HARMFUL_H
