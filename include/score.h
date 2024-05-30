#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef ARKANOID_SCORE_H
#define ARKANOID_SCORE_H

#include "ball.h"
#include "brick.h"
#include "vault.h"
#include "harmful.h"
#include "bonus.h"
#include "render.h"

#define MAX_HIGHSCORE 10
#define MAX_NAME_LENGTH 8

typedef struct
{
    char name[MAX_NAME_LENGTH + 1];
    int score;
} HighScore;

char playerName[MAX_NAME_LENGTH + 1] = "";
int currentScore = 0;

void showHighScores(SDL_Surface *win_surf, SDL_Surface *asciiSprites);
int compareHighScores(const void *a, const void *b);
void writeHighScores(HighScore highScores[], int count);
void sortHighScores(HighScore highScores[], int count);
void readHighScores(HighScore highScores[], int *count);
int getHighestScore();

#endif // ARKANOID_SCORE_H