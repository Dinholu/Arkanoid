#ifndef ARKANOID_BRICK_H
#define ARKANOID_BRICK_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define NUM_BRICKS_PER_ROW 16
#define NUM_ROWS 16

#define BRICK_WIDTH 32
#define BRICK_HEIGHT 16
#define FIRST_LINE 1

#define NUM_BRICKS (NUM_BRICKS_PER_ROW * NUM_ROWS)

#define BRICK(COIN, LIGNE) { (FIRST_LINE * COIN), (FIRST_LINE * LIGNE), BRICK_WIDTH, BRICK_HEIGHT }

#define WHITE_BRICK (SDL_Rect) BRICK(0, 0)
#define ORANGE_BRICK (SDL_Rect) BRICK(32, 0)
#define BLUE1_BRICK (SDL_Rect) BRICK(64, 0)
#define GREEN1_BRICK (SDL_Rect) BRICK(96, 0)
#define BLUE2_BRICK (SDL_Rect) BRICK(128, 0)
#define RED_BRICK (SDL_Rect) BRICK(0, 16)
#define BLUE3_BRICK (SDL_Rect) BRICK(32, 16)
#define PINK_BRICK (SDL_Rect) BRICK(64, 16)
#define YELLOW_BRICK (SDL_Rect) BRICK(96, 16)
#define GREY_BRICK (SDL_Rect) BRICK(0, 32)
#define GOLD_BRICK (SDL_Rect) BRICK(0, 48)

struct Brick {
    double x;
    double y;
    char type;
    int touched;
    int scoreValue;
    bool isVisible;
    bool isDestructible;
    bool isAnimating;
    int animationFrame;
    Uint64 lastFrameTime;
};
extern struct Brick brick[NUM_BRICKS];
extern SDL_Rect srcBrick;
extern int touched;

bool allBricksInvisible();

#endif // ARKANOID_BRICK_H
