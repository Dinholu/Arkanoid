#ifndef ARKANOID_GAME_H
#define ARKANOID_GAME_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "brick.h"
#include "score.h"
#include "render.h"
#include "bonus.h"
#include "input.h"
#include "doh.h"

#define NUM_LEVELS 33
#define VIE_MAX 5

typedef struct Level
{
    int bricks[NUM_ROWS][NUM_BRICKS_PER_ROW];
} Level;

extern Uint64 prev, now;
extern Level levels[NUM_LEVELS];
extern int currentLevel;
extern bool isGameOver;
extern bool showMenu;
extern int currentLife;
extern double delta_t;
extern const int FPS;

bool isCollision(SDL_Rect rect1, SDL_Rect rect2);
void loadCurrentLevel(bool isEight);
void loadLevelFromFile(const char *filename, bool isEigth);
void nextLevel();
void resetGame();
void mainGameLoop();

#endif // ARKANOID_GAME_H