#ifndef ARKANOID_DOH_H
#define ARKANOID_DOH_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "render.h"

typedef struct Doh
{
    double x;
    double y;
    int height;
    int width;
    char type;
    int health;
    int scoreValue;
} Doh;

extern Doh doh;
void initializeDoh();

#endif // ARKANOID_DOH_H