#ifndef ARKANOID_SCORE_H
#define ARKANOID_SCORE_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define MAX_HIGHSCORE 10
#define MAX_NAME_LENGTH 8

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    int score;
} HighScore;

extern char playerName[MAX_NAME_LENGTH + 1];
extern int currentScore;

int compareHighScores(const void *a, const void *b);
void writeHighScores(HighScore highScores[], int count);
void sortHighScores(HighScore highScores[], int count);
void readHighScores(HighScore highScores[], int *count);
int getHighestScore();
void saveHighScore(const char *playerName, int score);

#endif // ARKANOID_SCORE_H
