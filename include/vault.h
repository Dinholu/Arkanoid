#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifndef ARKANOID_VAULT_H
#define ARKANOID_VAULT_H

#include "ball.h"
#include "brick.h"
#include "bonus.h"
#include "harmful.h"
#include "render.h"

SDL_Rect srcVault = {384, 160, 82, 16};
SDL_Rect destVault;

int x_vault;
int vault_width;

void moveVault(const Uint8 *keys);

#endif // ARKANOID_VAULT_H
