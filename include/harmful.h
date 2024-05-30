#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef ARKANOID_HARMFUL_H
#define ARKANOID_HARMFUL_H

#include "ball.h"
#include "brick.h"
#include "vault.h"
#include "bonus.h"
#include "render.h"

#define HARMFUL_2 \
    (SDL_Rect) { 0, 256, 32, 32 }
#define HARMFUL_3 \
    (SDL_Rect) { 0, 288, 32, 32 }
#define HARMFUL_5 \
    (SDL_Rect) { 0, 320, 32, 32 }
#define HARMFUL_EXPLOSITION \
    (SDL_Rect) { 0, 384, 32, 32 }
#define MAX_HARMFULS 5

struct Harmful
{
    double x;
    double y;
    double vx;
    double vy;
    bool isActive;
    int type;
    bool isFalling;
    double time;
    int height; // Nouvelle variable pour la hauteur
    float elongationTime;
    double initialY;
    double amplitude;
    bool isSinusoidal;
    int animationFrame;
    double animationTime;
    float randonElongationTime;
    int maxSteps;
    bool isDestroying; // New field for tracking destruction
    int destroyAnimationFrame;
    double destroyAnimationTime;
} harmfuls[MAX_HARMFULS];
int currentHarmfulsType = 1;

void initializeHarmfuls();
void generateHarmfuls();
SDL_Rect getHarmfulSrcRect(int type, int frame);
void addRandomHarmfuls();
void handleHarmfulCollisions();
void clearHarmfuls();
#endif // ARKANOID_HARMFUL_H
