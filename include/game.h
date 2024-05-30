// GAME_H
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

#define NUM_LEVELS 33
#define VIE_MAX 5

typedef struct Level
{
    int bricks[NUM_ROWS][NUM_BRICKS_PER_ROW];
} Level;

Uint64 prev, now;
Level levels[NUM_LEVELS];

bool vWasPressed = false;
bool nwasPressed = false;
bool mWasPressed = false;

bool spaceWasPressed = false;
int currentLevel = 1;
bool enteringName = false;
bool isGameOver = false;
bool showMenu = true;
int nameIndex = 0;

int currentLife = 3;
double delta_t;
const int FPS = 60;

bool isCollision(SDL_Rect rect1, SDL_Rect rect2);
void loadCurrentLevel(bool isEigth);
void loadLevelFromFile(const char *filename, bool isEigth);
void processCongratulationsInput(SDL_Event *event);
void nextLevel();
void resetGame();
void mainGameLoop();

#endif // ARKANOID_GAME_H