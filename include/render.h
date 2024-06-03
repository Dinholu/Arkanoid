#ifndef ARKANOID_RENDER_H
#define ARKANOID_RENDER_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "bonus.h"
#include "harmful.h"

#define VAUS_HP (SDL_Rect) { 384, 118, 28, 10 }
#define MAX_BACKGROUND 5
#define ACTIVATE_SHADOW true

typedef struct
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
} Color;

extern SDL_Window *pWindow;
extern SDL_Surface *win_surf;

extern SDL_Surface *gameSprites;
extern SDL_Surface *asciiSprites;
extern SDL_Surface *menuSprites;
extern SDL_Surface *topWallSprites;
extern SDL_Surface *leftWallSprites;
extern SDL_Surface *rightWallSprites;

extern SDL_Rect srcBackground;
extern SDL_Rect srcLogo;
extern SDL_Rect srcVaus;
extern SDL_Rect srcTopWall;
extern SDL_Rect srcEdgeWall;

extern Color red;
extern Color green;
extern Color blue;
extern Color white;
extern Color grey;

extern const int Y_WALLS;
extern int backgroundChange;

void moveAndRenderHarmfuls(SDL_Surface *gameSprites, SDL_Surface *win_surf);
void moveAndRenderLasers(SDL_Surface *gameSprites, SDL_Rect *srcLeftLaser, SDL_Rect *srcRightLaser, SDL_Surface *win_surf);
void showHighScores(SDL_Surface *win_surf, SDL_Surface *asciiSprites);
SDL_Rect charToSDLRect(char character);
void renderString(SDL_Surface *sprites, SDL_Surface *surface, const char *string, int startX, int startY, const char *alignment, Color color);
void renderGameOverScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf);
void renderCongratulationsScreen(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf);
void renderMenu(SDL_Surface *sprites, SDL_Rect *srcLogo, SDL_Surface *win_surf);
void renderBackground(SDL_Surface *sprites, SDL_Rect *srcBackground, SDL_Surface *win_surf);
void changeBackground();
void renderBalls(SDL_Surface *sprites, SDL_Rect *srcBall, SDL_Surface *win_surf);
void renderVault(SDL_Surface *sprites, SDL_Rect *srcVault, SDL_Surface *win_surf, int x_vault);
void renderHP(SDL_Surface *sprites, SDL_Surface *win_surf, int currentLife);
void renderBricks(SDL_Surface *sprites, int num_bricks);
void renderInfo(SDL_Surface *sprites, int value, char *label, int startX, int startY, const char *alignment, Color color);
void showOptionsMenu(SDL_Window *pWindow, SDL_Surface *win_surf);
void initializeSDL();
void showPauseMenu(SDL_Surface *win_surf);
void renderWall(SDL_Surface *sprites, SDL_Rect *srcWall, int positionX, int positionY, int width, int height);
void renderAllWalls();
void render();
void renderBonuses(SDL_Surface *gameSprites, SDL_Surface *win_surf);
void renderShadow(SDL_Surface *surface, SDL_Rect *srcRect, SDL_Rect *destRect, int offsetX, int offsetY, int alpha);

#endif // ARKANOID_RENDER_H