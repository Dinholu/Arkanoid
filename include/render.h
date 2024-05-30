#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef ARKANOID_RENDER_H
#define ARKANOID_RENDER_H

#include "ball.h"
#include "brick.h"
#include "bonus.h"
#include "harmful.h"
#include "vault.h"
#include "score.h"

#define VAUS_HP \
    (SDL_Rect) { 384, 118, 28, 10 }
#define MAX_BACKGROUND 5

SDL_Window *pWindow = NULL;
SDL_Surface *win_surf = NULL;

SDL_Surface *plancheSprites = NULL;
SDL_Surface *gameSprites = NULL;
SDL_Surface *asciiSprites = NULL;
SDL_Surface *menuSprites = NULL;
SDL_Surface *topWallSprites = NULL;
SDL_Surface *leftWallSprites = NULL;
SDL_Surface *rightWallSprites = NULL;

SDL_Rect srcBackground = {0, 128, 64, 64};

SDL_Rect srcLogo = {0, 0, 388, 96};
SDL_Rect srcVaus = {0, 108, 192, 90};

// Mur
SDL_Rect srcTopWall = {22, 0, 512, 22};
SDL_Rect srcEdgeWall = {0, 0, 22, 650};
const int Y_WALLS = 144;

int backgroundChange = 0;
void moveAndRenderLasers(SDL_Surface *gameSprites, SDL_Rect *srcLeftLaser, SDL_Rect *srcRightLaser, SDL_Surface *win_surf);
void moveAndRenderHarmfuls(SDL_Surface *gameSprites, SDL_Surface *win_surf);
void moveAndRenderBonuses(SDL_Surface *gameSprites, SDL_Surface *win_surf);
void renderString(SDL_Surface *sprites, SDL_Surface *surface, const char *string, int startX, int startY, const char *alignment);
void renderGameOverScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf);
void renderCongratulationsScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf);
void renderMenu(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf);
void renderBackground(SDL_Surface *sprites, SDL_Rect *srcBackground, SDL_Surface *win_surf);
void changeBackground();
void renderBalls(SDL_Surface *sprites, SDL_Rect *srcBall, SDL_Surface *win_surf);
void renderVault(SDL_Surface *sprites, SDL_Rect *srcVault, SDL_Surface *win_surf, int x_vault);
void renderHP(SDL_Surface *sprites, SDL_Surface *win_surf, int currentLife);
void renderBricks(SDL_Surface *sprites, int num_bricks);
void renderInfo(SDL_Surface *sprites, int value, char *label, int startX, int startY, const char *alignement);
void showOptionsMenu(SDL_Window *pWindow, SDL_Surface *win_surf);
void initializeSDL();
void renderWall(SDL_Surface *sprites, SDL_Rect *srcWall, int positionX, int positionY, int width, int height);
void renderAllWalls();
void render();

#endif // ARKANOID_RENDER_H
