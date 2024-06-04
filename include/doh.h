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
    int animationFrame;
    Uint64 lastFrameTime;
    int animationPhase;
    Uint64 phaseStartTime;
    bool moveDown;
    int moveDownTime;
    Uint64 moveStartTime;
    bool hasMovedDown; // Indicateur pour s'assurer que le mouvement ne se produit qu'une fois
    bool disappearing; // Indicateur pour l'animation de disparition

} Doh;

extern Doh doh;
void initializeDoh();

#endif // ARKANOID_DOH_H